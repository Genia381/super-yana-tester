/****************************************************************
                SUPER YANA TESTER
                File: Voltmeter.ino

  Красивый экран вольтметра.
  Пока внешнего ADS1115 нет, показывает реальное напряжение
  нашей батарейки через getBatteryVoltage() / GP29.
  Долгое OK - назад в меню.
****************************************************************/

#include "Config.h"
#include <Wire.h>

//======================================================
// ADS1115 для вольтметра
// SDA = GP8, SCL = GP9, адрес = 0x48
// Делитель напряжения: 100 кОм / 10 кОм
//======================================================

#define ADS1115_ADDR 0x48
#define VOLTMETER_DIVIDER_RATIO 11.0f

// Пока коэффициент равен 1.000.
// Позже его будет менять настоящая калибровка вольтметра.
float voltmeterCalibration = 1.000f;

// Отдельная I2C-шина для ядра Arduino Mbed RP2040.
arduino::MbedI2C voltmeterWire(8, 9);

static bool voltmeterAdsStarted = false;
static bool voltmeterAdsPresent = false;

static bool adsWriteRegister(uint8_t reg, uint16_t value)
{
  voltmeterWire.beginTransmission(ADS1115_ADDR);
  voltmeterWire.write(reg);
  voltmeterWire.write((uint8_t)(value >> 8));
  voltmeterWire.write((uint8_t)(value & 0xFF));
  return voltmeterWire.endTransmission() == 0;
}

static bool adsReadRegister(uint8_t reg, uint16_t &value)
{
  voltmeterWire.beginTransmission(ADS1115_ADDR);
  voltmeterWire.write(reg);

  if (voltmeterWire.endTransmission(false) != 0)
    return false;

  if (voltmeterWire.requestFrom(ADS1115_ADDR, (uint8_t)2) != 2)
    return false;

  uint8_t highByte = voltmeterWire.read();
  uint8_t lowByte = voltmeterWire.read();

  value = ((uint16_t)highByte << 8) | lowByte;
  return true;
}

static void voltmeterAdsInit()
{
  if (voltmeterAdsStarted)
    return;

  voltmeterWire.begin();
  delay(10);

  voltmeterWire.beginTransmission(ADS1115_ADDR);
  voltmeterAdsPresent = (voltmeterWire.endTransmission() == 0);
  voltmeterAdsStarted = voltmeterAdsPresent;
}

bool adsReadSingleEndedRaw(uint8_t channel, int16_t &raw)
{
  voltmeterAdsInit();

  if (!voltmeterAdsPresent || channel > 3)
    return false;

  // Однократное измерение AIN0...AIN3 относительно GND.
  // MUX: A0=100, A1=101, A2=110, A3=111.
  // Диапазон: ±4.096 В, 1 отсчёт = 0.000125 В, 128 SPS.
  uint16_t mux = (uint16_t)(0x04 + channel) << 12;
  uint16_t config = 0x8000 | mux | 0x0383;

  if (!adsWriteRegister(0x01, config))
  {
    voltmeterAdsPresent = false;
    voltmeterAdsStarted = false;
    return false;
  }

  delay(9);

  uint16_t rawUnsigned;
  if (!adsReadRegister(0x00, rawUnsigned))
  {
    voltmeterAdsPresent = false;
    voltmeterAdsStarted = false;
    return false;
  }

  raw = (int16_t)rawUnsigned;
  return true;
}

//======================================================
// readVoltmeterValue()
//
// Возвращает напряжение до делителя.
// При ошибке ADS1115 возвращает -1.
//======================================================

float readVoltmeterValue()
{
  voltmeterAdsInit();

  if (!voltmeterAdsPresent)
    return -1.0f;

  const uint8_t samples = 12;
  int32_t sum = 0;
  uint8_t good = 0;

  for (uint8_t i = 0; i < samples; i++)
  {
    int16_t raw = 0;

    if (adsReadSingleEndedRaw(0, raw))
    {
      sum += raw;
      good++;
    }
  }

  if (good == 0)
  {
    voltmeterAdsPresent = false;
    voltmeterAdsStarted = false;
    return -1.0f;
  }

  float averageRaw = (float)sum / (float)good;
  float adcVoltage = averageRaw * 0.000125f;

  if (adcVoltage < 0.0f)
    adcVoltage = 0.0f;

  float inputVoltage =
      adcVoltage *
      VOLTMETER_DIVIDER_RATIO *
      voltmeterCalibration;

  // Убираем прыжки возле нуля.
  if (inputVoltage < 0.05f)
    inputVoltage = 0.0f;

  return inputVoltage;
}

//======================================================
// drawVoltmeterError()
//======================================================

void drawVoltmeterError()
{
  tft.fillRoundRect(10, 38, 140, 58, 6, 0x0186);
  tft.drawRoundRect(10, 38, 140, 58, 6, TFT_RED);

  tft.setTextColor(TFT_RED, 0x0186);
  tft.drawCentreString("ADS1115", CENTER_X, 47, 2);
  tft.drawCentreString("NOT FOUND", CENTER_X, 67, 2);

  tft.setTextColor(TFT_YELLOW, 0x0186);
  tft.drawCentreString("GP8 / GP9   0x48", CENTER_X, 86, 1);
}

//======================================================
// drawVoltmeterTitle()
//======================================================

void drawVoltmeterTitle()
{
  tft.setTextColor(TFT_YELLOW, COLOR_BG);

  if (languageMode == LANG_RU)
  {
    drawCyrTextCentered("VOLbTMETR", CENTER_X, 23, TFT_YELLOW); // ВОЛЬТМЕТР
  }
  else if (languageMode == LANG_UA)
  {
    drawCyrTextCentered("VOLbTMETR", CENTER_X, 23, TFT_YELLOW); // ВОЛЬТМЕТР
  }
  else
  {
    tft.drawCentreString("VOLTMETER", CENTER_X, 20, 2);
  }
}

//======================================================
// drawVoltmeterScale()
//======================================================

void drawVoltmeterScale(float voltage)
{
  const int x = 14;
  const int y = 82;
  const int w = 132;
  const int h = 14;

  // Шкала внешнего вольтметра ADS1115: 0...30 В.
  float v = voltage;
  if (v < 0.0f) v = 0.0f;
  if (v > 30.0f) v = 30.0f;

  int fillW = (int)((v / 30.0f) * (w - 4));
  if (fillW < 0) fillW = 0;
  if (fillW > (w - 4)) fillW = w - 4;

  tft.fillRoundRect(x, y, w, h, 4, TFT_DARKGREY);
  tft.drawRoundRect(x, y, w, h, 4, TFT_CYAN);

  uint16_t barColor = TFT_GREEN;
  if (voltage >= 24.0f) barColor = TFT_YELLOW;
  if (voltage >= 28.0f) barColor = TFT_RED;

  if (fillW > 0)
  {
    tft.fillRoundRect(x + 2, y + 2, fillW, h - 4, 3, barColor);
  }

  // Под шкалой больше нет делений. Только красная надпись максимума.
  tft.fillRect(18, 99, 124, 11, COLOR_BG);
  tft.setTextColor(TFT_RED, COLOR_BG);
  tft.drawCentreString("MAKC. 30V", CENTER_X, 99, 1);
}

//======================================================
// drawVoltmeterValue()
//======================================================

void drawVoltmeterValue(float voltage)
{
  // Основная карточка значения
  tft.fillRoundRect(10, 38, 140, 37, 6, 0x0186);
  tft.drawRoundRect(10, 38, 140, 37, 6, TFT_CYAN);

  uint16_t valueColor = TFT_BLUE;
  if (voltage >= 28.0f) valueColor = TFT_RED;

  tft.setTextColor(valueColor, 0x0186);
  tft.drawCentreString(String(voltage, 2), 70, 47, 4);

  tft.setTextColor(valueColor, 0x0186);
  tft.drawString("V", 122, 52, 2);
}

//======================================================
// drawVoltmeterScreen()
//======================================================

void drawVoltmeterScreen()
{
  clearScreen();

  drawStatusBar();
  drawFrame();
  drawBottomBar();

  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  drawVoltmeterTitle();

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedHoldOKMenu(CENTER_X, 116, TFT_GREEN);
}

//======================================================
// voltmeterLoop()
//======================================================

void voltmeterLoop()
{
  voltmeterAdsInit();
  drawVoltmeterScreen();

  unsigned long timer = 0;
  unsigned long batteryTimer = 0;

  while (true)
  {
    if (autoOffTick())
    {
      drawVoltmeterScreen();
      timer = 0;
      batteryTimer = 0;
    }

    if (buttonOKLong(2200))
    {
          drawMenu(true);
      return;
    }

    // Живое обновление верхней строки батарейки
    if (millis() - batteryTimer >= 1000)
    {
      batteryTimer = millis();
      drawStatusBar();
    }

    if (millis() - timer >= 250)
    {
      timer = millis();

      float voltage = readVoltmeterValue();

      if (voltage < 0.0f)
      {
        drawVoltmeterError();
      }
      else
      {
        drawVoltmeterValue(voltage);
        drawVoltmeterScale(voltage);
      }
    }
  }
}
