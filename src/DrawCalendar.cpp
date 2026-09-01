#include "DrawCalendar.h"

#include "EPD.h"

DrawCalendar::DrawCalendar() {

}

// --- Public functions ---

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

void DrawCalendar::setYearMonth(int year, int month) {
    year_ = year;
    month_ = month;
}

void DrawCalendar::display() {
    // 上半分
    Paint_Clear(WHITE0);
    drawCalendarFrame(0);
    drawWeekdays(0);
    PIC_display_Half1(image_);

    // 下半分
    Paint_Clear(WHITE0);
    drawCalendarFrame(HALF_HEIGHT);
    PIC_display_Half2(image_);
}

// --- end public functions ---

// --- Private functions ---
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

void DrawCalendar::drawWeekdays(int yOffset) {
    // 曜日表示の描画
    static const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    constexpr int calendarWidth = DRAW_RIGHT - DRAW_LEFT;
    constexpr int headerBottom = DRAW_TOP + HEADER_HEIGHT;

    for (int col = 0; col < GRID_COLS; col++) {
        int cellLeft = DRAW_LEFT + col * calendarWidth / GRID_COLS;
        int cellRight = DRAW_LEFT + (col + 1) * calendarWidth / GRID_COLS;
        int cellWidth = cellRight - cellLeft;

        // Font20で3文字分
        int textWidth = 3 * Font20.Width;

        // 各曜日セルの中央に配置
        int x = cellLeft + (cellWidth - textWidth) / 2;
        int y = headerBottom + (WEEKDAY_HEIGHT - Font20.Height) / 2;

        // 現在描画中の半画面に存在しなければ描画しない
        if (y < yOffset || y >= yOffset + HALF_HEIGHT) {
            continue;
        }

        UWORD color = (col == 0) ? RED0 : BLACK0; // 日曜日は赤色
        Paint_DrawString_EN(
            x, 
            y - yOffset,
            weekdays[col],
            &Font20,
            color,
            WHITE0
        );
    }
}

bool DrawCalendar::isLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

int DrawCalendar::getDaysInMonth(int year, int month) {
    static const int daysInMonth[] = {
        31, // January
        28, // February (non-leap year)
        31, // March
        30, // April
        31, // May
        30, // June
        31, // July
        31, // August
        30, // September
        31, // October
        30, // November
        31  // December
    };

    if (month < 1 || month > 12) {
        return -1; // Invalid month
    }

    if (month == 2 && isLeapYear(year)) {
        return 29; // February in a leap year
    }

    return daysInMonth[month - 1];
}

int DrawCalendar::getFirstWeekdayOfMonth(int year, int month) {
    // ツェラーの公式を用いて1日目の曜日を計算
    if (month < 3) {
        month += 12;
        year--;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (1 + (13 * (month + 1)) / 5 + k + (k / 4) + (j / 4) - (2 * j)) % 7;
    //       ^ day = 1

    // Convert to 0=Sun, 1=Mon, ..., 6=Sat
    return (h + 6) % 7;
}
// --- end private functions ---