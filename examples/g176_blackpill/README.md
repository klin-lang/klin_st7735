# G-176 + STM32F411CE Black Pill

Side-by-side sketches for the same wiring: Arduino (`hello_sd.ino`) and
Klin (`hello_sd.kl`). This folder is an **app** example. The chip driver
stays MCU-agnostic (`klin_st7735`); the bus is yours.

There is **no** WeAct Black Pill board pack yet. The Klin file is the
application. Flashing still needs a freestanding scaffold (`startup.s` /
linker) — `klin init nucleo-f411` is the closest published one. F411CE and
F411RE share the same `machine_stm32` MMIO. HSI 16 MHz after reset is
enough for first bring-up.

## Hardware

| Piece | Shop | Notes |
|---|---|---|
| 1.8" TFT ST7735S + SD slot | [Elektroweb G-176](https://elektroweb.pl/pl/wyswietlacze-lcd/535-wyswietlacz-lcd-tft-18-spi-st7735s-z-czytnikiem-kart-sd.html) | 128×160, 16-bit colour, SPI, not touch. Supply 3.3–5 V; **backlight LED pin wants 3.3 V**. 63 mm × 38 mm. Index **G-176**, EAN 5904162803774 |
| STM32 Black Pill | [Elektroweb J-094](https://elektroweb.pl/pl/stm32/778-mikrokontroler-stm32f411ceu6-stm32-blackpill.html) | WeAct-style **STM32F411CEU6**, Cortex-M4 FPU, 100 MHz, 512 KB flash, 128 KB RAM, USB-C OTG. LED + user button. Index **J-094**, EAN 5904162805457 |

Vendor extras for G-176 (EN manual, schematic, outline) are on the product
page under *Pliki*.

This driver talks to the **ST7735S** only (`Tab.black` = Adafruit
`INITR_BLACKTAB`). The SD slot is a second SPI device. There is no Klin
FatFs / `SD.h` package yet — both sketches keep `SD_CS` idle HIGH. The
Arduino file still calls `SD.begin` so you can see the Arduino shape;
the Klin file does not pretend that API exists.

## Wiring (SPI1)

Use **SPI1** so `SD_CS` is not MOSI. SPI2 MOSI is **PB15** — do not put
TFT MOSI and `SD_CS` on PB15 at once. Both chip-selects idle HIGH.

| G-176 | Black Pill F411CE |
|---|---|
| VCC | 3V3 |
| GND | GND |
| LED | 3V3 (or a GPIO later) |
| SCK / CLK | PA5 (SPI1 SCK, AF5) |
| SDA | PA7 (SPI1 MOSI, AF5) |
| A0 / RS / DC | PB0 |
| RESET | PB1 |
| CS | PB12 |
| SD_CS | PB15 (keep HIGH while talking to the TFT) |
| SD_MISO | PA6 (SPI1 MISO) |

Arduino `Serial` on this board is usually **USB CDC**. Klin
`machine_stm32` has **USART**, not USB — `hello_sd.kl` uses USART1
PA9/PA10 (USB-UART adapter).

## Arduino

Needs STM32duino (WeAct / Generic F411CE), Adafruit ST7735, and SD.

Arduino IDE wants a sketch folder named like the `.ino`. Open
`hello_sd.ino` and let the IDE move it, or copy the file to
`hello_sd/hello_sd.ino`.

Board: **Generic STM32F4 / Black Pill F411CE**. Upload via DFU or SWD.

## Klin

```sh
klin get github/klin-lang/machine_stm32@v0.5.0
# from the klin_st7735 repo root:
#   dart run path/to/klin/bin/klin.dart --emit-c -I. examples/g176_blackpill/hello_sd.kl
```

`--emit-c` checks the Klin. Linking an ELF needs `arm-none-eabi-gcc` plus
`@[link("startup.s")]` from a board scaffold (not in this folder).
`klin run hello_sd.kl` also loads sibling `wire.kl` (same `module app`).

### Why `hello_sd.kl` is not 40 lines of Adafruit

The work is the same. Arduino puts it in `Adafruit_ST7735` + `SPI` + `SD`
(thousands of lines you do not see). Klin will not hide SPI, CS, clocks, or
allocation behind a constructor.

`hello_sd.kl` is the **app** (like `setup`). `wire.kl` is the **adapter**
(like the Adafruit `.cpp`). A future board pack can shorten `spi_out(…)`
further; it cannot delete the bytes on the wire.

`draw_text` takes `[]i32` ASCII — no GFX cursor / `print(str)`. USB
`Serial` is not in `machine_stm32` (USART only).

| Arduino | Klin |
|---|---|
| `setup` / `loop` | `fn main()` + `while true` |
| `Adafruit_ST7735` + `initR(BLACKTAB)` | `lcd.attach(wire, lcd.Tab.black)` |
| `fillScreen` / `setCursor` / `print` | `fill` / `draw_text` (`[]i32` ASCII, no cursor) |
| hidden `SPI` | `machine.spi_out` + CS as `Pin` |
| `SD.begin` / `File` | not in this package — later, or `@[cimport]` FatFs |
| USB `Serial` | USART `write_u8` |

## Flash

**Klin does not flash.** Arduino “Upload” is STM32duino calling
`dfu-util`. The Klin equivalent is `klin init weact-f411` then `make flash`.

```sh
klin init weact-f411 g176_app
cd g176_app
# replace main.kl with hello_sd.kl + wire.kl (keep board/startup.s)
klin get
make
# hold BOOT0, tap NRST
make flash
```

`make flash` → `dfu-util`. `make flash-swd` → `st-flash`. USB-C is ROM DFU,
not an ST-Link. Needs a Klin that ships the `weact-f411` template.

## License

Same as the package (MIT). Shop links are the vendors' pages; this repo
does not sell the boards.
