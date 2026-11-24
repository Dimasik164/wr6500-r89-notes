# Wi-Fi prototype status

There is no upstream BCM6766 wireless driver in OpenWrt. The bench radios only operate with the owner's closed stock radio module plus the GPL prototype sources in `files/wifi/`. The closed module, stock userspace, board calibration, network names, and passphrase are not part of this repository.

## Source components

- `wlan_glue_mod.c` supplies pktfwd initialization and a missing multicast notifier.
- `0002-bcm-pktfwd-wlan-glue-4.19.patch` adjusts initialization and allocation context in the GPL pktfwd source.
- `awlpatch.c` prevents the vendor Archer WLAN bind path used by this build.
- `vunmapfix.c` changes two exact ARM instructions reached while the radio is brought up.
- `taskletguard.c` guards an exact tasklet call site whose function pointer was empty.
- `txfix.c` initializes the transmit tasklet data and calls the stock driver's queue worker.

Every item above is tied to the observed Linux 4.19.294 ARM build. The modules verify selected functions, instructions, or state before changing them, but they are not portable across kernel or closed-driver builds. `txfix.c` contains an offset into one closed-driver text layout; another build must be reverse-engineered and validated independently.

`txfix.ko` does not discover the closed driver's text base by itself. Read the loaded base from `/sys/module/wl/sections/.text` and pass that value as the module's `textbase` parameter; this source then adds `0x30d690`, an offset valid only for the tested `wl` build.

Do not load `awlunit.ko` or the stock `archer.ko`, and do not call `nvram kcommit`. Do not bring either radio interface up until the kernel log contains `vunmapfix: vunmap wrote`, `taskletguard: wrote`, and `rx: 0`. Those checks were required to avoid the crashes observed during bring-up.

## Bench result

The observed order was glue, Archer bypass, virtual-memory workaround, tasklet guard, the matching stock dependencies, the GPL PCIe host module, the closed radio module, and finally the transmit-queue workaround. The radios were then configured as 5 GHz on channel 36 with 80 MHz width and 2.4 GHz on channel 6.

On the laboratory unit, the matching stock authentication daemon reported a completed WPA2 four-way handshake on both radio interfaces. A client obtained a LAN address and exchanged ICMP traffic with the router in both directions. No kernel Oops was observed during that check.

This is not evidence for WPA3, MLO, 6 GHz, roaming, throughput, long-run stability, or operation with a different stock radio build.

## Build outline

Point `KDIR` at the built Cudy 4.19 kernel and `BCM_SRC` at `package/extra/bcm/src` in the Cudy GPL tree. Apply patch 0002 to the GPL pktfwd file and place that patched file beside the Makefile as `bcm_pktfwd.c` for the out-of-tree build. Do not commit the copied vendor file.

The patches under `patches/host/` are build-host compatibility fixes for automake, libtool, yacc, and OpenSSL 3 on a current workstation. They do not change the target device behavior.
