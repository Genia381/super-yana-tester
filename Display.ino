#include "Config.h"

//======================================================
// Очистка экрана
//======================================================

void clearScreen()
{
    tft.fillScreen(COLOR_BG);
}

//======================================================
// Верхняя строка
//======================================================

void drawStatusBar()
{
    tft.fillRect(0, 0, LCD_WIDTH, 16, TFT_DARKGREY);

    float batt = getBatteryVoltage();

    // Батарейка + вольты слева
    drawBattery(batt);

    // Справа вместо YANA показываем проценты заряда
    int percent = getBatteryPercent(batt);
    tft.fillRect(116, 1, 42, 14, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawRightString(String(percent) + "%", 156, 2, 2);
}

//======================================================
// Нижняя строка
//======================================================

void drawBottomBar()
{
    tft.fillRect(0, 112, LCD_WIDTH, 16, TFT_DARKGREY);
}

//======================================================
// Рамка
//======================================================

void drawFrame()
{
    tft.drawRect(0, 16, LCD_WIDTH, 96, COLOR_FRAME);

    // Уголки

    tft.drawFastHLine(0,16,12,COLOR_FRAME);
    tft.drawFastHLine(148,16,12,COLOR_FRAME);

    tft.drawFastHLine(0,111,12,COLOR_FRAME);
    tft.drawFastHLine(148,111,12,COLOR_FRAME);
}

//======================================================
// Заголовок
//======================================================

void drawTitle(const char *txt)
{
    tft.setTextColor(TFT_GREEN, COLOR_BG);

    tft.drawCentreString(txt,80,20,2);
}

//======================================================
// Очистка рабочей области
//======================================================

void clearWorkArea()
{
    tft.fillRect(2,18,156,92,COLOR_BG);
}

//======================================================
// Фоновая сетка
//======================================================

void drawGrid()
{
    uint16_t c = COLOR_GRID;

    for(int x=0;x<160;x+=10)
        tft.drawFastVLine(x,16,96,c);

    for(int y=16;y<112;y+=10)
        tft.drawFastHLine(0,y,160,c);
}

//======================================================
// Центральное сообщение
//======================================================

void centerText(const char *txt)
{
    tft.setTextColor(TFT_WHITE,COLOR_BG);

    tft.drawCentreString(txt,80,60,2);
}

//======================================================
// Большое сообщение
//======================================================

void bigText(const char *txt)
{
    tft.setTextColor(TFT_YELLOW,COLOR_BG);

    tft.drawCentreString(txt,80,50,4);
}

//======================================================
// Главный экран
//======================================================

void drawMainScreen()
{
    clearScreen();

    drawStatusBar();

    drawFrame();

    drawGrid();

    drawBottomBar();

    drawTitle(txt3("YANA MULTITESTER", "YANA TESTER", "YANA TESTER"));
}
