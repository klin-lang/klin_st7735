# klin_st7735

Klin chip driver for **ST7735S** over 4-wire SPI.

Not Adafruit GFX, not Arduino, not in the Klin stdlib. The app owns the
bus (`machine_*` `Spi` + `Pin`, or any other C/Klin hooks). This package
sends commands and RGB565 pixels.

Default panel: **128×160** Adafruit `INITR_BLACKTAB` — the cheap 1.8"
ST7735S modules with an SD slot (e.g. Elektroweb G-176).

## Requirements

- [Klin](https://github.com/klin-lang/klin) compiler

## Install

```sh
klin get github/MrHIDEn/klin_st7735@v0.1.0
```

Repo: https://github.com/MrHIDEn/klin_st7735  
Same layout as `klin-lang/*` packages; transfer to the org when you have
create rights there. Local use: `-I` this tree or a sibling `klin_st7735/`.

## API (`@v0.1.0`)

| Symbol | Meaning |
|---|---|
| `version(): i32` | `1` at `v0.1.0` |
| `Tab.black` | 128×160, origin 0, MADCTL `0xC8` (MX\|MY\|BGR) |
| `geom_for(tab)` | width / height / window offsets / MADCTL |
| `rgb565` / `color_*` | RGB565, high byte first on the wire (Adafruit order) |
| `Wire` | `cmd` / `data` / `delay_ms` / `rst` + `ctx` (no capture) |
| `attach(wire, tab)` | hardware reset + init |
| `fill` / `fill_rect` / `pixel` / `hline` / `vline` | clip to the panel |

No font, no sprites, no SD, no DMA, no heap. A later tag can add a
bulk-write hook; today `fill` is one RGB565 word per pixel.

### Wire

```klin
import klin_st7735 lcd

fn spi_cmd(ctx: *mut u8, b: i32) {
    // DC low, CS low, write b, CS high
}

fn spi_data(ctx: *mut u8, b: i32) {
    // DC high, CS low, write b, CS high
}

fn wait_ms(ctx: *mut u8, ms: i32) {
    // busy-wait or timer — caller cost
}

fn pulse_rst(ctx: *mut u8) {
    // RST high / low / high with delays
}

fn main() {
    let wire = lcd.Wire{
        ctx: cast(*mut u8, 0),
        cmd: spi_cmd,
        data: spi_data,
        delay_ms: wait_ms,
        rst: pulse_rst
    }
    let tft = lcd.attach(wire, lcd.Tab.black)
    tft.fill(lcd.color_black())
    tft.fill_rect(0, 0, 32, 16, lcd.color_red())
}
```

`ctx` is how you pass `Spi` / `Pin` without closures. See
[`eventloop`](https://github.com/klin-lang/eventloop) for the same shape.

## Host checks

```sh
# from this repo, Klin on PATH:
klin test klin_st7735
klin run -I. examples/host_smoke.kl
```

## Next: G-176 + STM32F411CE Black Pill

A board example is a **later** package (or example tree), not this chip
driver.

| Piece | Status |
|---|---|
| ST7735S protocol | this package |
| STM32F411 SPI / GPIO | [`machine_stm32`](https://github.com/klin-lang/machine_stm32) `@v0.5.0` — **no compiler change** |
| Nucleo-F411RE scaffold | `klin init nucleo-f411` + [`nucleo_f411re`](https://github.com/klin-lang/nucleo_f411re) |
| WeAct Black Pill F411CE | **not** a Klin board pack yet (LED PC13, KEY PA0, HSE 25 MHz, USB-C) |

F411CE and F411RE are the same F411-class MMIO. `machine_stm32` already
talks SPI1/SPI2. What the Black Pill still needs is a **board pack**
(pins, clock if you want 100 MHz from the 25 MHz crystal, flash via
SWD/DFU). For a first bring-up, HSI 16 MHz after reset is enough —
same as the Nucleo examples.

Suggested wiring (SPI1, so `SD_CS` is not MOSI):

| G-176 | Black Pill F411CE |
|---|---|
| VCC | 3V3 (module is 3.3–5 V; backlight LED wants 3.3 V) |
| GND | GND |
| LED | 3V3 (or a GPIO later) |
| SCK / CLK | PA5 (SPI1 SCK, AF5) |
| SDA | PA7 (SPI1 MOSI, AF5) |
| A0 / RS / DC | PB0 |
| RESET | PB1 |
| CS | PA4 or PB12 |
| SD_CS | PB15 (later; keep HIGH while talking to the TFT) |
| SD_MISO | PA6 (SPI1 MISO) |

Do **not** put TFT MOSI and `SD_CS` on PB15 at once (SPI2 MOSI is
PB15). Shared SPI + two chip-selects, both idle HIGH.

SD / FatFs is a separate package. This driver does not touch the slot.

## Layout

Directory `klin_st7735/` is one module. `*_test.kl` stays out of
`import`.

## License

MIT
