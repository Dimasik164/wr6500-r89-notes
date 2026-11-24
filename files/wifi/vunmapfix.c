// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <asm/cacheflush.h>

static int apply;
module_param(apply, int, 0644);

#define ARM_NOP 0xe320f000u

struct fix {
	const char *name;
	unsigned off;
	u32 expect;
};

static const struct fix fixes[] = {
	{ "vunmap", 0x20, 0x1a000002 },
	{ "__get_vm_area_node", 0x34, 0x1a000043 },
};

static int __init vunmapfix_init(void)
{
	void (*text_rw)(void);
	void (*text_ro)(void);
	long (*probe_write)(void *, const void *, size_t);
	unsigned i;

	text_rw = (void *)kallsyms_lookup_name("set_kernel_text_rw");
	text_ro = (void *)kallsyms_lookup_name("set_kernel_text_ro");
	probe_write = (void *)kallsyms_lookup_name("probe_kernel_write");
	if (!text_rw || !text_ro || !probe_write) {
		pr_err("vunmapfix: text write helpers missing\n");
		return -ENOENT;
	}
	for (i = 0; i < ARRAY_SIZE(fixes); i++) {
		unsigned long fn = kallsyms_lookup_name(fixes[i].name);
		unsigned long insn;
		u32 old;

		if (!fn) {
			pr_err("vunmapfix: %s missing\n", fixes[i].name);
			return -ENOENT;
		}
		insn = fn + fixes[i].off;
		old = *(u32 *)insn;
		pr_err("vunmapfix: %s %lx word %08x\n", fixes[i].name, insn, old);
		if (old != fixes[i].expect && old != ARM_NOP) {
			return -EINVAL;
		}
	}
	if (apply == 2) {
		unsigned long fn = kallsyms_lookup_name(fixes[1].name);
		unsigned long insn = fn + fixes[1].off;
		u32 same = *(u32 *)insn;
		long rc = probe_write((void *)insn, &same, sizeof(same));
		pr_err("vunmapfix: probe same-value rc %ld\n", rc);
		return rc;
	}
	if (apply == 3) {
		unsigned long fn = kallsyms_lookup_name(fixes[1].name);
		unsigned long insn = fn + fixes[1].off;
		u32 same = *(u32 *)insn;
		long rc;
		text_rw();
		rc = probe_write((void *)insn, &same, sizeof(same));
		text_ro();
		pr_err("vunmapfix: rw probe same-value rc %ld\n", rc);
		return rc;
	}
	if (apply == 1) {
		text_rw();
		for (i = ARRAY_SIZE(fixes); i-- > 0;) {
			unsigned long fn = kallsyms_lookup_name(fixes[i].name);
			unsigned long insn = fn + fixes[i].off;
			u32 patch = ARM_NOP, now;
			long rc;
			rc = probe_write((void *)insn, &patch, sizeof(patch));
			if (rc) {
				pr_err("vunmapfix: %s probe write %ld\n", fixes[i].name, rc);
				text_ro();
				return rc;
			}
			flush_icache_range(insn, insn + 4);
			now = *(u32 *)insn;
			pr_err("vunmapfix: %s wrote %08x\n", fixes[i].name, now);
			if (now != ARM_NOP) {
				text_ro();
				return -EIO;
			}
		}
		text_ro();
	}
	return 0;
}

static void __exit vunmapfix_exit(void)
{
}

module_init(vunmapfix_init);
module_exit(vunmapfix_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Allow vunmap from the Broadcom radio up path");
