# SPDX-License-Identifier: GPL-2.0-only
# Out-of-tree build of wlan_glue against the Cudy GPL kernel.
# Copy the patched bcm_pktfwd.c beside this Makefile. Do not commit that copy.
# See docs/wifi-bringup.md.

BCM_SRC ?=
KDIR ?=

ifeq ($(BCM_SRC),)
$(error Set BCM_SRC to package/extra/bcm/src from the Cudy GPL tree)
endif

obj-m := wlan_glue.o awlpatch.o vunmapfix.o taskletguard.o txfix.o
wlan_glue-y := wlan_glue_mod.o bcm_pktfwd.o

ccflags-y += -DBCM_PKTFWD -DCONFIG_BCM_WLAN_16BIT_STATION_CHAIN_IDX_SUPPORT
ccflags-y += -I$(BCM_SRC)/bcmdrivers/opensource/include/bcm963xx
ccflags-y += -I$(BCM_SRC)/bcmdrivers/opensource/char/bcmlibs/include
ccflags-y += -I$(BCM_SRC)/bcmdrivers/broadcom/include/bcm963xx
ccflags-y += -I$(BCM_SRC)/kernel/bcmkernel/include
ccflags-y += -I$(BCM_SRC)/kernel/bcmkernel/include/uapi
ccflags-y += -I$(BCM_SRC)/shared/opensource/include/bcm963xx
ccflags-y += -I$(BCM_SRC)/kernel/linux-4.19/net/bridge
ccflags-y += -Wno-date-time -Wno-declaration-after-statement -Wno-unused-function

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
