# Cudy WR6500 R89 vendor-tree notes

This repository is a small, reviewable source catalog for a **Cudy WR6500 EU1.0 R89** based on Broadcom BCM6766. It does not apply to the WR6500 H revision.

The material is derived from Cudy's GPL release for OpenWrt 21.02 and Linux 4.19.294. It is not an OpenWrt target, a firmware image, a sysupgrade image, or a supported installation method.

## Scope

- The SPI-NAND patch adds the ESMT F50L1G41LC ID used by this board to Cudy's older Broadcom 4.19 driver.
- The patch was sufficient to read the stock UBI headers on one bench unit. NAND program and erase safety were not tested.
- The launcher loads the Archer and `bcm_enet` modules built by the same Cudy GPL tree. These vendor objects are not an upstream Ethernet driver.
- Wi-Fi support is not included. No proprietary firmware, calibration data, credentials, binaries, images, or NAND dumps are included.
- The only repeatable boot path documented here is UART, TFTP, and `bootm` into RAM. It does not write NAND.

## Contents

- `patches/0001-bcm6766-spinand-f50l1g41lc-4.19.patch` — vendor Linux 4.19 SPI-NAND ID patch.
- `files/bcm_boot_launcher` — module loader used by the RAM-only initramfs.
- `docs/partition-map.md` — measured stock MTD and UBI layout.
- `docs/repeat-ram-boot.md` — high-level RAM-only reproduction path.
- `docs/upstream-gap.md` — why this is not a current OpenWrt port.
- `COPYING` — GPL-2.0 license text from the Cudy OpenWrt kernel tree.

The patch and launcher are licensed **GPL-2.0-only**.

## References

- Cudy GPL source archive: https://drive.google.com/file/d/1HSkfoc-IUqcReapLygwi5dQpm-vvw1mW/view
- OpenWrt forum topic: https://forum.openwrt.org/t/cudy-wr6500-v1-0-r89-bcm6766-openwrt-porting-help/255609

