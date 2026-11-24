// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <asm/cacheflush.h>

static int apply;
module_param(apply, int, 0644);
MODULE_PARM_DESC(apply, "1 makes crossbow_wlan_bind_hook return -2 immediately");

#define EXPECT_PUSH 0xe92d4ff0u
#define PATCH_MVN   0xe3e00001u
#define PATCH_BXLR  0xe12fff1eu

static int __init awlpatch_init(void)
{
	unsigned long hook, page;
	int (*set_rw)(unsigned long, int);
	u32 *insn;
	int rc;

	hook = kallsyms_lookup_name("crossbow_wlan_bind_hook");
	if (!hook) {
		pr_err("awlpatch: hook missing\n");
		return -ENOENT;
	}
	insn = (u32 *)hook;
	pr_err("awlpatch: hook %lx words %08x %08x apply %d\n",
	       hook, insn[0], insn[1], apply);
	if (insn[0] == PATCH_MVN && insn[1] == PATCH_BXLR)
		return 0;
	if (insn[0] != EXPECT_PUSH) {
		pr_err("awlpatch: unexpected prologue\n");
		return -EINVAL;
	}
	if (!apply)
		return 0;
	set_rw = (void *)kallsyms_lookup_name("set_memory_rw");
	if (!set_rw) {
		pr_err("awlpatch: set_memory_rw missing\n");
		return -ENOENT;
	}
	page = hook & PAGE_MASK;
	rc = set_rw(page, 1);
	if (rc) {
		pr_err("awlpatch: set_memory_rw %d\n", rc);
		return rc;
	}
	insn[0] = PATCH_MVN;
	insn[1] = PATCH_BXLR;
	flush_icache_range(hook, hook + 8);
	pr_err("awlpatch: wrote %08x %08x\n", insn[0], insn[1]);
	if (insn[0] != PATCH_MVN || insn[1] != PATCH_BXLR)
		return -EIO;
	return 0;
}

static void __exit awlpatch_exit(void)
{
}

module_init(awlpatch_init);
module_exit(awlpatch_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("WR6500 Archer WLAN bind disable");
