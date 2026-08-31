#include <ESP32epdx.h>
#include <SPI.h>

#include "EPD.h"
#include "DrawCalendar.h"

#define EPD_BUSY A14
#define EPD_RST  A15
#define EPD_DC   A16
#define EPD_CS   A17

DrawCalendar calendar;

void setup() {
    pinMode(EPD_BUSY, INPUT);
    pinMode(EPD_RST, OUTPUT);
    pinMode(EPD_DC, OUTPUT);
    pinMode(EPD_CS, OUTPUT);

    SPI.begin(
        EPD_SCK,   // SCK
        -1,        // MISO unused
        EPD_MOSI,  // MOSI / SDI
        EPD_CS     // CS
    );

    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

    // カレンダー初期化
    calendar.begin();

    // カレンダー描画
    EPD_init();
    calendar.display();
    EPD_DeepSleep();
}

void loop() {
    
}