# E-Paper Calender
## Overview

## Setup
ESP32epdxのライブラリを利用します．`include/`に`GUI/*.h`, `EPD_SPI.h`, `EPD.h`, `EPS32epdx.h`, `IMAGE.h`を，`src`に`GUI/*.cpp`, `EPD_SPI.cpp`, `EPD.cpp`を配置してください．また`platformio.ini`の`build_flags`はESP32-C3用にピン定義をしています．使用するボードに合わせて適宜書き換えてください．