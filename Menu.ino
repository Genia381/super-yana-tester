/****************************************************************
                YANA MULTITESTER
                File: Menu.ino

  Главное меню.

  Исправлено:
  - пункт Info оставлен;
  - пункт Exit добавлен после Info;
  - пункты меню не впихиваются в экран, а прокручиваются;
  - заголовок и первый пункт меню разведены по высоте;
  - OK на Exit из главного меню запускает сохранённый режим.
****************************************************************/

#include "Config.h"

//======================================================
// НАСТРОЙКИ РАСПОЛОЖЕНИЯ МЕНЮ
//======================================================

#define MENU_TITLE_Y       20
#define MENU_FIRST_Y       42
#define MENU_STEP          16

// На экране показываем 4 пункта. Остальное листается.
#define MENU_VISIBLE_COUNT 4

#define MENU_TEXT_X        32
#define MENU_CURSOR_X      16
#define MENU_SELECT_X      8
#define MENU_SELECT_W      144
#define MENU_SELECT_H      16

#define MENU_CLEAR_X       2
#define MENU_CLEAR_Y       38
#define MENU_CLEAR_W       156
#define MENU_CLEAR_H       72

//======================================================
// ПУНКТЫ ГЛАВНОГО МЕНЮ
//======================================================

const uint8_t MENU_COUNT = 7;
const uint8_t MENU_INFO_INDEX = 5;
const uint8_t MENU_EXIT_INDEX = 6;

uint8_t menuIndex = 0;
uint8_t oldMenuIndex = 255;
uint8_t menuTopIndex = 0;
uint8_t oldMenuTopIndex = 255;

//======================================================
// updateMenuScroll()
//
// Держит выбранный пункт внутри видимой области.
//======================================================

void updateMenuScroll()
{
  if (menuIndex < menuTopIndex)
    menuTopIndex = menuIndex;

  if (menuIndex >= menuTopIndex + MENU_VISIBLE_COUNT)
    menuTopIndex = menuIndex - MENU_VISIBLE_COUNT + 1;

  if (menuTopIndex + MENU_VISIBLE_COUNT > MENU_COUNT)
  {
    if (MENU_COUNT > MENU_VISIBLE_COUNT)
      menuTopIndex = MENU_COUNT - MENU_VISIBLE_COUNT;
    else
      menuTopIndex = 0;
  }
}

//======================================================
// drawMenuScrollMarks()
//======================================================

void drawMenuScrollMarks()
{
  tft.fillRect(150, MENU_CLEAR_Y, 8, MENU_CLEAR_H, COLOR_BG);
  tft.setTextColor(TFT_CYAN, COLOR_BG);

  if (menuTopIndex > 0)
    tft.drawString("^", 151, MENU_CLEAR_Y + 2, 1);

  if (menuTopIndex + MENU_VISIBLE_COUNT < MENU_COUNT)
    tft.drawString("v", 151, MENU_CLEAR_Y + MENU_CLEAR_H - 10, 1);
}

//======================================================
// drawVisibleMenuItem()
//======================================================

void drawVisibleMenuItem(uint8_t screenLine, uint8_t itemIndex, bool selected)
{
  int y = MENU_FIRST_Y + screenLine * MENU_STEP;

  tft.fillRect(MENU_CLEAR_X, y - 1, MENU_CLEAR_W - 8, MENU_SELECT_H + 1, COLOR_BG);

  if (selected)
  {
    tft.fillRect(MENU_SELECT_X, y, MENU_SELECT_W, MENU_SELECT_H, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", MENU_CURSOR_X, y + 1, 2);
    drawLocalizedMenuItem(itemIndex, MENU_TEXT_X, y + 1, (itemIndex == MENU_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (itemIndex == MENU_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedMenuItem(itemIndex, MENU_TEXT_X, y + 1, textColor);
  }
}

//======================================================
// drawMenuItems()
//======================================================

void drawMenuItems()
{
  updateMenuScroll();

  tft.fillRect(MENU_CLEAR_X, MENU_CLEAR_Y, MENU_CLEAR_W, MENU_CLEAR_H, COLOR_BG);

  uint8_t visible = MENU_VISIBLE_COUNT;
  if (MENU_COUNT < visible)
    visible = MENU_COUNT;

  for (uint8_t line = 0; line < visible; line++)
  {
    uint8_t item = menuTopIndex + line;
    drawVisibleMenuItem(line, item, item == menuIndex);
  }

  drawMenuScrollMarks();

  oldMenuIndex = menuIndex;
  oldMenuTopIndex = menuTopIndex;
}

//======================================================
// drawMenu()
//======================================================

void drawMenu(bool fullRedraw)
{
  updateMenuScroll();

  if (fullRedraw)
  {
    clearScreen();
    drawStatusBar();
    drawFrame();
    drawBottomBar();

    // Чистим только внутреннюю область рамки.
    tft.fillRect(1, 17, 158, 94, COLOR_BG);

    // Заголовок отдельно от списка, чтобы Tester не налезал на MAIN MENU.
    tft.setTextColor(TFT_YELLOW, COLOR_BG);
    drawLocalizedMainMenuTitle(CENTER_X, MENU_TITLE_Y, TFT_YELLOW);

    drawMenuItems();

    tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
    drawLocalizedOKSelectHint(CENTER_X, 116, TFT_GREEN);
    return;
  }

  if (oldMenuIndex != menuIndex || oldMenuTopIndex != menuTopIndex)
  {
    drawMenuItems();
  }
}

//======================================================
// menuInit()
//======================================================

void menuInit()
{
  menuIndex = 0;
  menuTopIndex = 0;
  oldMenuIndex = 255;
  oldMenuTopIndex = 255;
  drawMenu(true);
}

//======================================================
// showScreenStub()
//======================================================

void showScreenStub(const char *title)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();

  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawCentreString(title, CENTER_X, 43, 2);

  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawCentreString(txtComingSoon(), CENTER_X, 64, 2);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  tft.setTextColor(TFT_RED, COLOR_BG);
  tft.drawCentreString(txtExit(), CENTER_X, 88, 2);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  tft.drawCentreString(txtMainMenuHint(), CENTER_X, 116, 1);

  unsigned long batteryTimer = 0;

  while (true)
  {
    if (autoOffTick())
    {
      drawMenu(true);
      return;
    }

    if (millis() - batteryTimer >= 1000)
    {
      batteryTimer = millis();
      drawStatusBar();
    }

    if (buttonOK())
    {
      drawMenu(true);
      return;
    }

    if (buttonOKLong(2200))
    {
      drawMenu(true);
      return;
    }

    delay(10);
  }
}


//======================================================
// infoAutoOffText()
//======================================================

const char* infoAutoOffText()
{
  switch (autoOffMode)
  {
    case 0: return "1 MIN";
    case 1: return "2 MIN";
    case 2: return "5 MIN";
    default: return "OFF";
  }
}

//======================================================
// infoStartupText()
//======================================================

const char* infoStartupText()
{
  return (startupMode == STARTUP_VOLTMETER) ? "VOLTMETER" : "TESTER";
}

//======================================================
// drawInfoScreenStatic()
//======================================================

void drawInfoScreenStatic()
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();

  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_CYAN, COLOR_BG);
  tft.drawCentreString("SUPER YANA TESTER", CENTER_X, 22, 1);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  tft.drawCentreString("v2 alpha7 fix54", CENTER_X, 33, 1);

  tft.drawFastHLine(10, 45, 140, TFT_DARKGREY);

  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawString("BAT:", 14, 51, 1);
  tft.drawString("LANG:", 14, 63, 1);
  tft.drawString("BRIGHT:", 14, 75, 1);
  tft.drawString("AUTO OFF:", 14, 87, 1);
  tft.drawString("START:", 14, 99, 1);

  drawLocalizedHoldOKMenu(CENTER_X, 113, TFT_GREEN);
}

//======================================================
// drawInfoScreenValues()
//======================================================

void drawInfoScreenValues()
{
  char buf[18];

  float bat = getBatteryVoltage();
  snprintf(buf, sizeof(buf), "%.2fV", bat);

  tft.fillRect(78, 51, 70, 58, COLOR_BG);

  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawString(buf, 86, 51, 1);

  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawString(languageName(), 86, 63, 1);
  tft.drawString(displayBrightnessName(), 86, 75, 1);
  tft.drawString(infoAutoOffText(), 86, 87, 1);
  tft.drawString(infoStartupText(), 86, 99, 1);
}

//======================================================
// infoScreenLoop()
//======================================================

void infoScreenLoop()
{
  soundMode();
  drawInfoScreenStatic();
  drawInfoScreenValues();

  unsigned long infoTimer = 0;

  while (true)
  {
    if (autoOffTick())
    {
      drawMenu(true);
      return;
    }

    if (millis() - infoTimer >= 1000)
    {
      infoTimer = millis();
      drawStatusBar();
      drawInfoScreenValues();
    }

    if (buttonOK() || buttonOKLong(2200))
    {
      drawMenu(true);
      return;
    }

    delay(10);
  }
}

//======================================================
// openMenuItem()
//======================================================

void openMenuItem()
{
  switch (menuIndex)
  {
    case 0:
      soundMode();
      testerLoop();
      break;

    case 1:
      soundMode();
      probeLoop();
      break;

    case 2:
      soundMode();
      voltmeterLoop();
      break;

    case 3:
      soundMode();
      irRemoteLoop();
      break;

    case 4:
      settingsLoop();
      break;

    case MENU_INFO_INDEX:
      infoScreenLoop();
      break;

    case MENU_EXIT_INDEX:
      // Exit из главного меню = запуск сохранённого режима.
      soundMode();
      startSelectedStartupMode();
      drawMenu(true);
      break;
  }
}

//======================================================
// menuLoop()
//======================================================

void menuLoop()
{
  static unsigned long batteryTimer = 0;

  if (autoOffTick())
  {
    drawMenu(true);
  }

  // Живое обновление батарейки в главном меню
  if (millis() - batteryTimer >= 1000)
  {
    batteryTimer = millis();
    drawStatusBar();
  }

  if (buttonUp())
  {
    if (menuIndex == 0)
      menuIndex = MENU_COUNT - 1;
    else
      menuIndex--;

    drawMenu(false);
  }

  if (buttonDown())
  {
    menuIndex++;

    if (menuIndex >= MENU_COUNT)
      menuIndex = 0;

    drawMenu(false);
  }

  if (buttonOK())
  {
    openMenuItem();
  }
}
