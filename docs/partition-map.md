# Stock partition and UBI map

This is a **snapshot of the original stock layout observed before later laboratory changes**. It is reference data, not a description of the board's current contents and not a flashing plan.

## MTD layout

| MTD | Size | Name | Range |
|---|---:|---|---|
| mtd0 | `0x0800_0000` | `spi1.0` | whole 128 MiB chip |
| mtd1 | `0x0020_0000` | `loader` | `0x0000_0000`–`0x0020_0000` |
| mtd2 | `0x0004_0000` | `bdinfo` | `0x0020_0000`–`0x0024_0000` |
| mtd3 | `0x07cc_0000` | `image` | `0x0024_0000`–`0x07f0_0000` |

The final 1 MiB, `0x07f0_0000`–`0x0800_0000`, was outside the observed `mtdparts` layout and is deliberately unnamed.

## UBI layout

The stock snapshot had 11 user volumes across 998 LEBs (120.8 MiB), with 94 LEBs available, no bad PEBs, and a 2,048-byte minimum I/O unit. Image 2 was selected; `bootfs2` and `rootfs2` were active.

| ID | Name | Type | Reserved size | Observed data / mount |
|---:|---|---|---|---|
| 1 | `metadata1` | static | 1 LEB, 124.0 KiB | 272 bytes |
| 2 | `metadata2` | static | 1 LEB, 124.0 KiB | 272 bytes |
| 3 | `bootfs1` | static | 28 LEB, 3.3 MiB | 3,522,112 bytes |
| 4 | `rootfs1` | dynamic | 182 LEB, 22.0 MiB | |
| 5 | `bootfs2` | static | 36 LEB, 4.3 MiB | 3,522,104 bytes |
| 6 | `rootfs2` | dynamic | 203 LEB, 24.5 MiB | stock SquashFS via `ubiblock0_6` |
| 7 | `uboot1` | static | 15 LEB, 1.8 MiB | 1,867,794 bytes |
| 8 | `uboot2` | static | 15 LEB, 1.8 MiB | 1,867,794 bytes |
| 10 | `data` | dynamic | 166 LEB, 20.1 MiB | 19,808,256 bytes; mounted as 18 MiB / 156 LEBs |
| 11 | `defaults` | dynamic | 67 LEB, 8.1 MiB | 7,237,632 bytes; mounted as 6 MiB / 57 LEBs |
| 21 | `rootfs_data` | dynamic | 166 LEB, 20.1 MiB | 19,808,256 bytes; mounted as 18 MiB / 156 LEBs |

There was no volume 9 and no volume 12–20. Filesystem UUIDs and device-specific data are intentionally omitted. The vendor DTS did not contain this table; it was reconstructed from the stock kernel layout and `ubinfo` output.

