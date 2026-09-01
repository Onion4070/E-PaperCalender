# E-Paper Calender
## Overview

## Setup
ESP32epdxのライブラリを利用します．`include/`に`GUI/*.h`, `EPD_SPI.h`, `EPD.h`, `EPS32epdx.h`, `IMAGE.h`を，`src`に`GUI/*.cpp`, `EPD_SPI.cpp`, `EPD.cpp`を配置してください．また`platformio.ini`の`build_flags`はESP32-C3用にピン定義をしています．使用するボードに合わせて適宜書き換えてください．

### Reqire modification
`GUI_Paint.cpp`内の`Paint_DrawPoint()`にオフセットの問題が含まれることを確認しました．
264行目以降の`Dot_Style == DOT_FILL_AROUND`がヒットする際のfor文内を以下のように変更してください．
```cpp
if (Dot_Style == DOT_FILL_AROUND) {
    for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
        for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
            int x = Xpoint + XDir_Num - Dot_Pixel + 1;
            int y = Ypoint + YDir_Num - Dot_Pixel + 1;
            if (x < 0 || y < 0) {
                continue;
            }
            Paint_SetPixel(x, y, Color);
        }
    }
}
```