// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/netdevice.h>

/*
 * wl_start queues a frame, then schedules the tasklet at wl+0x50.
 * That tasklet's function was never set, so the queue sticks and
 * txq_dispatched stays 1. The kthread calls wl_start_txqwork(wl+0xa0),
 * which loads its wl pointer from wl+0xb0. That word is also unset,
 * so waking the kthread faults at lock wl+0x90.
 *
 * Point the tasklet at a small function that fills wl+0xb0 and runs
 * the driver's own wl_start_txqwork. f4 stays 0: that path uses
 * spinlocks and does not sleep.
 */

static unsigned long textbase;
module_param(textbase, ulong, 0444);

static void (*txqwork)(void *task);

struct txfix_arm {
	int used;
	void *wl;
	void *old_func;
	unsigned long old_data;
};

static struct txfix_arm arms[2];

static void txfix_task(unsigned long data)
{
	void *wl = (void *)data;

	WRITE_ONCE(*(void **)((unsigned char *)wl + 0xb0), wl);
	txqwork((unsigned char *)wl + 0xa0);
}

static int arm_one(const char *name, int idx)
{
	struct net_device *dev;
	void *wlif;
	void *wl;
	u8 disp, f4;
	u32 qcnt, tcount;

	dev = dev_get_by_name(&init_net, name);
	if (!dev)
		return -ENODEV;
	wlif = READ_ONCE(*(void **)((unsigned char *)dev + 0x580));
	wl = (wlif && (unsigned long)wlif >= 0xc0000000UL) ?
		READ_ONCE(*(void **)((unsigned char *)wlif + 8)) : NULL;
	if (!wl || (unsigned long)wl < 0xc0000000UL) {
		pr_err("txfix: %s bad wl\n", name);
		dev_put(dev);
		return -EINVAL;
	}
	disp = READ_ONCE(*(u8 *)((unsigned char *)wl + 0x8c));
	f4 = READ_ONCE(*(u8 *)((unsigned char *)wl + 0xf4));
	qcnt = READ_ONCE(*(u32 *)((unsigned char *)wl + 0x9c));
	tcount = READ_ONCE(*(u32 *)((unsigned char *)wl + 0x58));
	pr_err("txfix: %s disp %u f4 %u qcnt %u tcount %u\n",
	       name, disp, f4, qcnt, tcount);
	if (f4 || tcount) {
		dev_put(dev);
		return -EBUSY;
	}
	arms[idx].wl = wl;
	arms[idx].old_func = READ_ONCE(*(void **)((unsigned char *)wl + 0x5c));
	arms[idx].old_data = READ_ONCE(*(unsigned long *)((unsigned char *)wl + 0x60));
	arms[idx].used = 1;
	WRITE_ONCE(*(void **)((unsigned char *)wl + 0xb0), wl);
	smp_wmb();
	WRITE_ONCE(*(unsigned long *)((unsigned char *)wl + 0x60), (unsigned long)wl);
	smp_wmb();
	WRITE_ONCE(*(void **)((unsigned char *)wl + 0x5c), txfix_task);
	if (disp || qcnt)
		txqwork((unsigned char *)wl + 0xa0);
	disp = READ_ONCE(*(u8 *)((unsigned char *)wl + 0x8c));
	qcnt = READ_ONCE(*(u32 *)((unsigned char *)wl + 0x9c));
	dev_put(dev);
	pr_err("txfix: %s armed disp %u qcnt %u\n", name, disp, qcnt);
	return 0;
}

static void disarm_one(int idx)
{
	void *wl;

	if (!arms[idx].used)
		return;
	wl = arms[idx].wl;
	WRITE_ONCE(*(void **)((unsigned char *)wl + 0x5c), arms[idx].old_func);
	smp_wmb();
	WRITE_ONCE(*(unsigned long *)((unsigned char *)wl + 0x60), arms[idx].old_data);
	arms[idx].used = 0;
}

static int __init txfix_init(void)
{
	int rc;

	if (textbase < 0xc0000000UL)
		return -EINVAL;
	txqwork = (void *)(textbase + 0x30d690UL);
	pr_err("txfix: txqwork offset ok\n");
	rc = arm_one("wl0", 0);
	if (rc)
		return rc;
	rc = arm_one("wl1", 1);
	if (rc)
		disarm_one(0);
	return rc;
}

static void __exit txfix_exit(void)
{
	disarm_one(0);
	disarm_one(1);
}

module_init(txfix_init);
module_exit(txfix_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Run WR6500 wl_start_txqwork from the idle tx tasklet");

