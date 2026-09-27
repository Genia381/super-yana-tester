/****************************************************************
                SUPER YANA TESTER
                File: Probe.ino

  ПРОБНИК / ПРОЗВОНКА на ADS1115 A1.
  Схема: 3.3V -> 1 кОм -> красный щуп -> Rx -> GND.
  A1 подключён к красному щупу через защитный резистор 4.7 кОм.
****************************************************************/

#include "Config.h"

#define PROBE_RREF_OHM       1000.0f
#define PROBE_SUPPLY_V       3.300f
#define PROBE_BEEP_LIMIT     20.0f
#define PROBE_FAST_LIMIT     50.0f
#define PROBE_OPEN_LIMIT     5000.0f
#define PROBE_OPEN_VOLTAGE   2.700f
#define PROBE_ZERO_OFFSET    0.0f

static float readProbeResistance()
{
  const uint8_t samples = 8;
  int32_t sum = 0;
  uint8_t good = 0;

  for (uint8_t i = 0; i < samples; i++)
  {
    int16_t raw = 0;
    if (adsReadSingleEndedRaw(1, raw))
    {
      if (raw < 0) raw = 0;
      sum += raw;
      good++;
    }
  }

  if (good == 0)
    return -1.0f;

  float voltage = ((float)sum / (float)good) * 0.000125f;

  // На собранной схеме холостой вход может быть ниже 3.3 В
  // из-за защитных диодов и их утечки. Всё выше 2.7 В считаем обрывом.
  if (voltage >= PROBE_OPEN_VOLTAGE)
    return PROBE_OPEN_LIMIT + 1.0f;

  float denominator = PROBE_SUPPLY_V - voltage;
  if (denominator <= 0.001f)
    return PROBE_OPEN_LIMIT + 1.0f;

  float resistance = PROBE_RREF_OHM * voltage / denominator;
  resistance -= PROBE_ZERO_OFFSET;

  if (resistance < 0.0f)
    resistance = 0.0f;

  return resistance;
}

static void drawProbeTitle()
{
  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  if (languageMode == LANG_EN)
    tft.drawCentreString("PROBE", CENTER_X, 20, 2);
  else
    drawCyrTextCentered("PROBNIK", CENTER_X, 23, TFT_YELLOW);
}

static void drawProbeStatic()
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);
  drawProbeTitle();

  tft.fillRoundRect(10, 40, 140, 54, 7, 0x0186);
  tft.drawRoundRect(10, 40, 140, 54, 7, TFT_CYAN);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedHoldOKMenu(CENTER_X, 116, TFT_GREEN);
}

static void drawProbeValue(float resistance)
{
  tft.fillRoundRect(11, 41, 138, 52, 6, 0x0186);

  if (resistance < 0.0f)
  {
    tft.setTextColor(TFT_RED, 0x0186);
    tft.drawCentreString("ADS1115", CENTER_X, 49, 2);
    tft.drawCentreString("NOT FOUND", CENTER_X, 69, 2);
    return;
  }

  if (resistance > PROBE_OPEN_LIMIT)
  {
    tft.setTextColor(TFT_YELLOW, 0x0186);
    if (languageMode == LANG_EN)
      tft.drawCentreString("OPEN", CENTER_X, 56, 4);
    else
      drawCyrTextCentered("OBRYV", CENTER_X, 62, TFT_YELLOW);
    return;
  }

  uint16_t c = TFT_WHITE;
  if (resistance <= PROBE_BEEP_LIMIT) c = TFT_GREEN;
  else if (resistance <= PROBE_FAST_LIMIT) c = TFT_YELLOW;

  String value;
  if (resistance < 10.0f) value = String(resistance, 2);
  else if (resistance < 100.0f) value = String(resistance, 1);
  else if (resistance < 1000.0f) value = String(resistance, 0);
  else value = String(resistance / 1000.0f, 2);

  tft.setTextColor(c, 0x0186);
  tft.drawCentreString(value, 72, 49, 4);
  tft.setTextColor(c, 0x0186);
  tft.drawString((resistance < 1000.0f) ? "Ohm" : "k", 120, 58, 2);
}

void probeLoop()
{
  drawProbeStatic();

  unsigned long measureTimer = 0;
  unsigned long batteryTimer = 0;
  unsigned long pulseTimer = 0;

  while (true)
  {
    if (autoOffTick())
    {
      digitalWrite(BUZZER_PIN, LOW);
      drawProbeStatic();
      measureTimer = 0;
      batteryTimer = 0;
    }

    if (buttonOKLong(2200))
    {
      digitalWrite(BUZZER_PIN, LOW);
      drawMenu(true);
      return;
    }

    if (millis() - batteryTimer >= 1000)
    {
      batteryTimer = millis();
      drawStatusBar();
    }

    if (millis() - measureTimer >= 120)
    {
      measureTimer = millis();
      float r = readProbeResistance();
      drawProbeValue(r);

      if (r >= 0.0f && r <= PROBE_BEEP_LIMIT)
      {
        soundTone(2400, 35);
      }
      else if (r > PROBE_BEEP_LIMIT && r <= PROBE_FAST_LIMIT)
      {
        if (millis() - pulseTimer >= 260)
        {
          pulseTimer = millis();
          soundTone(1900, 25);
        }
      }
      else
      {
        digitalWrite(BUZZER_PIN, LOW);
      }
    }

    delay(2);
  }
}
