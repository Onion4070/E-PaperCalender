#ifndef DRAW_CALENDAR_H
#define DRAW_CALENDAR_H

#define EPD_WIDTH   960 
#define EPD_HEIGHT  640
#define EPD_ARRAY  EPD_WIDTH*EPD_HEIGHT/4   

#include <ESP32epdx.h>

// Draw呼び出し前にEPD_Init()を，呼び出し後にEPD_DeepSleep()を呼ぶこと
class DrawCalendar {
public:
    DrawCalendar();

    // 初期化
    void begin();
    
    void setYearMonth(int year, int month);

    // カレンダーの枠を描画してEPDへ送る
    void display();


private:
    // Frame buffer
    unsigned char image_[EPD_ARRAY / 2];

    static constexpr int HALF_HEIGHT = EPD_HEIGHT / 2;

    // カレンダーレイアウト
    static constexpr int MARGIN_X = 5;      // 左右の余白
    static constexpr int MARGIN_Y = 5;      // 上下の余白

    static constexpr int DRAW_LEFT = MARGIN_X;
    static constexpr int DRAW_RIGHT = EPD_WIDTH - 1 - MARGIN_X;
    static constexpr int DRAW_TOP = MARGIN_Y;
    static constexpr int DRAW_BOTTOM = EPD_HEIGHT - 1 - MARGIN_Y;

    // 年月表示領域
    static constexpr int HEADER_HEIGHT = 60; 

    // 曜日表示領域
    static constexpr int WEEKDAY_HEIGHT = 40;

    // 日付グリッド開始位置
    static constexpr int GRID_TOP = DRAW_TOP + HEADER_HEIGHT + WEEKDAY_HEIGHT;

    static constexpr int GRID_COLS = 7;
    static constexpr int GRID_ROWS = 6;

    // 年月
    int year_ = 1900;
    int month_ = 1;

    // 内部描画関数
    void drawLineGlobal(
        int x1,
        int y1,
        int x2,
        int y2,
        int yOffset
    );

    void drawCalendarFrame(
        int yOffset
    );

    void drawWeekdays(int yOffset);

    bool isLeapYear(int year);
    int getDaysInMonth(int year, int month);
    int getFirstWeekdayOfMonth(int year, int month);
};

#endif