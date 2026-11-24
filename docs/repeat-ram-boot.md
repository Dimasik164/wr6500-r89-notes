# Repeat the RAM-only boot

This is a high-level description of the tested development path. It intentionally stops short of installation and contains no NAND write procedure.

1. Start with Cudy's GPL tree for OpenWrt 21.02 / Linux 4.19 and select `brcmbca / bcm96766 / DEVICE_R89`.
2. Apply `patches/0001-bcm6766-spinand-f50l1g41lc-4.19.patch`.
3. Build an initramfs containing the matching tree-built modules and install `files/bcm_boot_launcher` as `/usr/sbin/bcm_boot_launcher`.
4. Include these modules in the tested order: `bcmlibs`, `bcm_knvram`, `bcm_mpm`, `cmdlist`, `pktflow`, `bcm_bpm`, `bcmvlan`, `bcm_bp3drv`, `bcm_archer_gpl`, `archer`, `gdx`, `bcm_enet`, `bcmmcast`.
5. From the bootloader console, transfer the development FIT to RAM with TFTP and start it with `bootm`.
6. Verify the RAM environment only. During the original test, the untouched signed stock slot was still present and selected, so a normal reboot returned to stock. That statement is historical and is no longer true for the laboratory board: it was changed later, and its stock Image 2 slot was removed. Do not assume a reboot path on another unit without independently verifying its slots and metadata.

The tested FIT load address was `0x0200_0000`. Do not run NAND erase, NAND write, `sysupgrade`, or the stock web updater from this procedure. The patch demonstrates a read result only.

