// G-176 ST7735S + WeAct STM32F411CE Black Pill (Arduino).
// Wiring and Klin twin: README.md in this folder.
//
// https://elektroweb.pl/pl/wyswietlacze-lcd/535-wyswietlacz-lcd-tft-18-spi-st7735s-z-czytnikiem-kart-sd.html
// https://elektroweb.pl/pl/stm32/778-mikrokontroler-stm32f411ceu6-stm32-blackpill.html

#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <SD.h>

#define TFT_CS   PB12
#define TFT_DC   PB0
#define TFT_RST  PB1
#define SD_CS    PB15

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);

  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed");
    return;
  }
  Serial.println("SD ok");

  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Hello, SD!");
}

void loop() {
  // Read a file from the SD card and draw it (app-owned).
}
