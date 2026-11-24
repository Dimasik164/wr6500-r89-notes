# Upstream gap

These files are notes for Cudy's vendor tree, not an upstream OpenWrt port.

- Current OpenWrt 25.12.6 uses Linux 6.12 and musl, while this catalog targets Cudy's OpenWrt 21.02 / Linux 4.19.294 tree.
- OpenWrt has no BCM6766 target for this board. A publishable image would still need a mainline Ethernet driver, a device tree, and a safe installation path.
- Current mainline SPI-NAND support already recognizes manufacturer `0x8c`, device `0x2c`; the relevant OpenWrt work was merged in PR 20962.
- Do not port the vendor patch's eight-argument `NAND_MEMORG` line into current OpenWrt. The modern driver has a different API and implementation.
- Archer and `bcm_enet` here are vendor-tree modules, not substitutes for an upstream network driver.
- The GPL Wi-Fi prototype sources are in this catalog, but the radios only operate together with the matching closed `wl` module from this exact vendor build. That object is not included and the prototype is not an upstream wireless driver.

The reusable evidence is limited to the observed device identity, stock layout, vendor module order, and a RAM-only bench result.

