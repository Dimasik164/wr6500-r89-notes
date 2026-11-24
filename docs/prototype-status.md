# Prototype status

One Cudy WR6500 EU1.0 R89 laboratory unit runs a prototype derived from Cudy's OpenWrt 21.02 / Linux 4.19.294 GPL tree. This repository does not contain its firmware image or an installation method.

Ethernet, the observed port mapping, LEDs, the long-press reset handler, and both radio interfaces were exercised. WPA2 client association and bidirectional LAN ICMP traffic were observed on both bands after the transmit-queue workaround was added.

The result still depends on vendor Archer and Ethernet objects plus the owner's matching closed radio module. WAN internet access, WPA3, MLO, 6 GHz, sustained throughput, and long-run stability are not established.

The board was changed locally after the stock partition snapshot was recorded. Details of that change and any NAND-write procedure are intentionally outside this source catalog. For another unit, the documented repeatable path remains RAM-only UART, TFTP, and `bootm`.
