# Ports, LEDs, and buttons

Observed on the R89 bench unit. Factory MAC bytes are omitted.

## Ethernet

| Linux | Switch | Faceplate |
| --- | --- | --- |
| eth0 | internal port 0 | WAN |
| eth1 | external port 0 | LAN4 |
| eth2 | external port 1 | LAN3 |
| eth3 | external port 2 | LAN2 |
| eth4 | external port 3 | LAN1 |

`br-lan` is `192.168.10.1/24` and contains `eth1` through `eth4`. `eth0` is the WAN port and is not a member of that bridge.

The vendor device tree labels an external SF2 switch, EGPHY ports 0–3, management port 8, and `phy_serdes0` as `2500Base-X`. U-Boot reported a 2.5 Gbit/s serdes link. Linux traffic through that port was not measured.

On this chassis `eth2` at 100 Mbit/s repeatedly goes up and down. That is an observation, not a driver patch. The other LAN ports were not all occupied during that check.

## LEDs

Sysfs names from `96766SV1.dts`, active-low: `oem:green:lan1` through `lan4`, `oem:green:wan`, `oem:green:wan2`, `oem:green:internet`, `oem:red:internet`, `oem:green:status`, `oem:green:wifi`, `oem:green:wps`.

`files/wr6500-leds.init` binds lan1 to eth4, lan2 to eth3, lan3 to eth2, lan4 to eth1, and wan to eth0, each for link plus transmit and receive. Green internet follows eth0 link only. Red internet is set once at start: on when eth0 has no carrier, off when it does. It does not watch the cable after that. Status uses the heartbeat trigger. `wan2`, `wifi`, and `wps` stay off. The Wi-Fi LED does not reflect the radio.

## Buttons

`gpio-keys-polled`: reset on `gpioc` 18, active-low; WPS on `gpioc` 30, active-low. The external switch reset in the same file is `gpioc` 27, active-low, and was not exercised. The bound driver is `gpio_button_hotplug`.

`files/rc.button/reset` ignores a short press. A five-second hold erases the writable OpenWrt overlay and reboots; it does not select or restore a different firmware slot.
