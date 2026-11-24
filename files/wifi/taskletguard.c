// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <asm/cacheflush.h>

static int apply = 1;
module_param(apply, int, 0644);

static unsigned long patched_site;

#define EXPECT_BLX 0xe12fff33u
#define SITE_OFF 0x60u

extern void tasklet_null_tramp(void);

/* Eight-byte push keeps the 8-aligned SP that blx r3 had. */
asm(
".text\n"
".arm\n"
".globl tasklet_null_tramp\n"
"tasklet_null_tramp:\n"
"push {r4, lr}\n"
"cmp r3, #0\n"
"beq 1f\n"
"blx r3\n"
"1:\n"
"pop {r4, pc}\n"
);

static u32 make_bl(unsigned long from, unsigned long to)
{
	long off = (long)to - (long)(from + 8);

	return 0xeb000000u | (((u32)(off >> 2)) & 0x00ffffffu);
}

static int __init taskletguard_init(void)
{
	unsigned long fn, site, dest;
	void (*text_rw)(void);
	void (*text_ro)(void);
	long (*probe_write)(void *, const void *, size_t);
	u32 old, bl, now;
	long off;

	fn = kallsyms_lookup_name("tasklet_action_common.constprop.0");
	if (!fn) {
		pr_err("taskletguard: function missing\n");
		return -ENOENT;
	}
	site = fn + SITE_OFF;
	old = *(u32 *)site;
	pr_err("taskletguard: site %lx word %08x\n", site, old);
	if (old != EXPECT_BLX) {
		pr_err("taskletguard: unexpected word\n");
		return -EINVAL;
	}
	if (!apply)
		return 0;
	off = (long)(unsigned long)tasklet_null_tramp - (long)(site + 8);
	if (off > 0x1fffffc || off < -0x2000000) {
		pr_err("taskletguard: trampoline out of range\n");
		return -ERANGE;
	}
	text_rw = (void *)kallsyms_lookup_name("set_kernel_text_rw");
	text_ro = (void *)kallsyms_lookup_name("set_kernel_text_ro");
	probe_write = (void *)kallsyms_lookup_name("probe_kernel_write");
	if (!text_rw || !text_ro || !probe_write) {
		pr_err("taskletguard: text write helpers missing\n");
		return -ENOENT;
	}
	bl = make_bl(site, (unsigned long)tasklet_null_tramp);
	dest = site + 8 + (((long)((int)(bl << 8) >> 6)));
	if (dest != (unsigned long)tasklet_null_tramp) {
		pr_err("taskletguard: encode mismatch\n");
		return -EINVAL;
	}
	if (apply == 2) {
		long rc;
		text_rw();
		rc = probe_write((void *)site, &old, sizeof(old));
		text_ro();
		pr_err("taskletguard: rw probe same-value rc %ld\n", rc);
		return rc;
	}
	text_rw();
	if (probe_write((void *)site, &bl, sizeof(bl))) {
		text_ro();
		pr_err("taskletguard: probe write failed\n");
		return -EFAULT;
	}
	flush_icache_range(site, site + 4);
	text_ro();
	now = *(u32 *)site;
	pr_err("taskletguard: wrote %08x\n", now);
	if (now != bl)
		return -EIO;
	patched_site = site;
	return 0;
}

static void __exit taskletguard_exit(void)
{
	void (*text_rw)(void);
	void (*text_ro)(void);
	long (*probe_write)(void *, const void *, size_t);
	u32 now;

	if (!patched_site)
		return;
	text_rw = (void *)kallsyms_lookup_name("set_kernel_text_rw");
	text_ro = (void *)kallsyms_lookup_name("set_kernel_text_ro");
	probe_write = (void *)kallsyms_lookup_name("probe_kernel_write");
	if (!text_rw || !text_ro || !probe_write)
		return;
	text_rw();
	if (probe_write((void *)patched_site, &(u32){ EXPECT_BLX }, sizeof(u32))) {
		text_ro();
		return;
	}
	flush_icache_range(patched_site, patched_site + 4);
	text_ro();
	now = *(u32 *)patched_site;
	pr_err("taskletguard: restored %08x\n", now);
	patched_site = 0;
}

module_init(taskletguard_init);
module_exit(taskletguard_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Skip a tasklet whose function pointer is NULL");
