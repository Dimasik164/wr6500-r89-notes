// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/netdevice.h>

int bcm_pktfwd_sys_init(void);
void bcm_pktfwd_sys_fini(void);

/*
 * dhd.ko resolves this from bcmmcast when WLAN+BLOG are compiled in.
 * The running bcmmcast was built without that export. Returning 0 keeps
 * client disconnect from updating multicast snooping and does not touch
 * the live ethernet modules.
 */
int bcm_mcast_wlan_client_disconnect_notifier(struct net_device *dev, const char *mac)
{
	(void)dev;
	(void)mac;
	return 0;
}
EXPORT_SYMBOL(bcm_mcast_wlan_client_disconnect_notifier);

static int __init wlan_glue_init(void)
{
	int rc = bcm_pktfwd_sys_init();
	if (rc)
		return -EIO;
	pr_info("wlan_glue: pktfwd ready\n");
	return 0;
}

static void __exit wlan_glue_exit(void)
{
	bcm_pktfwd_sys_fini();
}

module_init(wlan_glue_init);
module_exit(wlan_glue_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("WR6500 WLAN pktfwd exports for stock dhd");
