# Cudy WR6500 R89 vendor-tree notes

This repository is a small, reviewable source catalog for the **Cudy WR6500 EU1.0 R89** (Broadcom BCM6766), not the H revision. It contains notes and GPL source written around Cudy's OpenWrt 21.02 / Linux 4.19.294 tree. It is not an upstream OpenWrt target, a firmware image, a sysupgrade image, or an installation procedure.

## Verified scope

- The ESMT F50L1G41LC patch is for Cudy's older Broadcom 4.19 SPI-NAND driver. It was sufficient to read UBI headers; program and erase safety are not established by that result.
- Ethernet was brought up with the vendor-tree Archer and `bcm_enet` modules. Those objects are not an upstream Ethernet driver and are not included here.
- Both radio interfaces and a completed WPA2 four-way handshake were observed on the bench after applying the glue and transmit-queue workarounds in `files/wifi/`. The closed stock radio module is still required and is not included.
- The Wi-Fi sources patch exact functions and structure offsets from one 4.19 build. They are diagnostic prototype code, not portable kernel fixes.
- No proprietary modules, radio firmware, calibration data, credentials, device identifiers, images, dumps, or NAND-write procedure are included.
- The only repeatable boot path documented here remains UART, TFTP, and `bootm` into RAM.

## Contents

- `patches/0001-bcm6766-spinand-f50l1g41lc-4.19.patch` — vendor Linux 4.19 SPI-NAND ID patch.
- `patches/0002-bcm-pktfwd-wlan-glue-4.19.patch` — local pktfwd changes used by the glue module.
- `patches/host/010` through `019` — build-host compatibility fixes for automake, libtool, yacc, and OpenSSL 3; they do not change the target device.
- `files/bcm_boot_launcher` — vendor-tree Ethernet module loader.
- `files/wifi/` — GPL glue, guards, transmit-queue workaround, and out-of-tree Makefile.
- `files/wr6500-leds.init` and `files/rc.button/reset` — observed LED mapping and long-press reset handlers.
- `docs/partition-map.md` — measured stock MTD and UBI layout.
- `docs/prototype-status.md` — concise status and limitations of the tested laboratory prototype.
- `docs/repeat-ram-boot.md` — high-level RAM-only reproduction path.
- `docs/wifi-bringup.md` — exact limitations and bench evidence for the radio prototype.
- `docs/board-ports.md` — observed Ethernet, LED, and button mapping.
- `docs/upstream-gap.md` — why this is not a current OpenWrt port.

The original code in this catalog is licensed **GPL-2.0-only**. See `COPYING`.

## References

- Cudy GPL source archive: https://drive.google.com/file/d/1HSkfoc-IUqcReapLygwi5dQpm-vvw1mW/view
- OpenWrt forum topic: https://forum.openwrt.org/t/cudy-wr6500-v1-0-r89-bcm6766-openwrt-porting-help/255609
