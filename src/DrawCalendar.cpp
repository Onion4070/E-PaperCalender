#include "DrawCalendar.h"

#include "EPD.h"

DrawCalendar::DrawCalendar() {
}

// バッファ初期化
void DrawCalendar::begin() {
    Paint_NewImage(
        image_,
        EPD_WIDTH,
        EPD_HEIGHT / 2,
        0,
        WHITE0
    );

    // 4 color
    Paint_SetScale(4);
    Paint_SelectImage(image_);
}

// Public drawing function
void DrawCalendar::display() {
    // 上半分
    Paint_Clear(WHITE0);
    drawCalendarFrame(0);
    PIC_display_Half1(image_);

    // 下半分
    Paint_Clear(WHITE0);
    drawCalendarFrame(HALF_HEIGHT);
    PIC_display_Half2(image_);
}


// Global coordinate -> half-buffer coordinate
void DrawCalendar::drawLineGlobal(
    int x1,
    int y1,
    int x2,
    int y2,
    int yOffset
)
{
    // Horizontal line
    if (y1 == y2) {
        
        // 現在の半画面に存在しない
        if (y1 < yOffset || y1 >= yOffset + HALF_HEIGHT) {
            return;
        }

        int localY = y1 - yOffset;

        Paint_DrawLine(
            x1,
            localY,
            x2,
            localY,
            BLACK0,
            LINE_STYLE_SOLID,
            DOT_PIXEL_1X1
        );

        return;
    }

    // Vertical line
    if (x1 == x2) {
        // 現在の半画面と交差している部分だけ取り出す
        int startY = max(y1, yOffset);

        int endY = min(y2, yOffset + HALF_HEIGHT - 1);

        // 交差しない
        if (startY > endY) {
            return;
        }

        Paint_DrawLine(
            x1,
            startY - yOffset,
            x2,
            endY - yOffset,
            BLACK0,
            LINE_STYLE_SOLID,
            DOT_PIXEL_1X1
        );
    }
}


// Draw calendar frame
void DrawCalendar::drawCalendarFrame(int yOffset) {
    // カレンダーの横幅
    constexpr int calendarWidth = DRAW_RIGHT - DRAW_LEFT;

    // 日付グリッド部分の高さ
    constexpr int calendarHeight = DRAW_BOTTOM - GRID_TOP;

    // Outer frame
    
    // Top
    drawLineGlobal(
        DRAW_LEFT,
        DRAW_TOP,
        DRAW_RIGHT,
        DRAW_TOP,
        yOffset
    );
    
    // Bottom
    drawLineGlobal(
        DRAW_LEFT,
        DRAW_BOTTOM,
        DRAW_RIGHT,
        DRAW_BOTTOM,
        yOffset
    );
    
    // Left
    drawLineGlobal(
        DRAW_LEFT,
        DRAW_TOP,
        DRAW_LEFT,
        DRAW_BOTTOM,
        yOffset
    );
    
    // Right
    drawLineGlobal(
        DRAW_RIGHT,
        DRAW_TOP,
        DRAW_RIGHT,
        DRAW_BOTTOM,
        yOffset
    );

    // Header
    
    // 年月欄と曜日欄の境界
    constexpr int headerBottom = DRAW_TOP + HEADER_HEIGHT;

    drawLineGlobal(
        DRAW_LEFT,
        headerBottom,
        DRAW_RIGHT,
        headerBottom,
        yOffset
    );

    // Weekday header

    // 曜日欄と日付グリッドの境界
    drawLineGlobal(
        DRAW_LEFT,
        GRID_TOP,
        DRAW_RIGHT,
        GRID_TOP,
        yOffset
    );

    // Day columns
    // ヘッダ部分は7分割しない
    for (int col = 1; col < GRID_COLS; col++) {
        int x = DRAW_LEFT + col * calendarWidth / GRID_COLS;

        drawLineGlobal(
            x,
            headerBottom,
            x,
            DRAW_BOTTOM,
            yOffset
        );
    }

    // Week rows
    for (int row = 1; row < GRID_ROWS; row++) {
        int y = GRID_TOP + row * calendarHeight / GRID_ROWS;

        drawLineGlobal(
            DRAW_LEFT,
            y,
            DRAW_RIGHT,
            y,
            yOffset
        );
    }
}
