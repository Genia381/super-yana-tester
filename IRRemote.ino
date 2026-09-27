/****************************************************************
                SUPER YANA TESTER v2
                File: IRRemote.ino

  IR Remote mode v2.

  Что делает:
  - сначала ловит RAW-импульсы с ИК-приёмника на IR_PIN;
  - если похоже на NEC — показывает NEC код;
  - если протокол другой — показывает RAW, чтобы было видно,
    что приёмник и пульт работают.

  Подключение:
  IR receiver OUT -> GP16 / IR_PIN
  VCC             -> 3.3V
  GND             -> GND
****************************************************************/

#include "Config.h"

//======================================================
// Настройки RAW / NEC
//======================================================

#define IR_RAW_MAX             90
#define IR_GAP_US              22000UL
#define IR_START_TIMEOUT_MS    5

#define NEC_LEAD_LOW_MIN       6500
#define NEC_LEAD_LOW_MAX       11000
#define NEC_LEAD_HIGH_MIN      2800
#define NEC_LEAD_HIGH_MAX      6500

#define NEC_REPEAT_HIGH_MIN    1400
#define NEC_REPEAT_HIGH_MAX    3500

#define NEC_BIT_LOW_MIN        200
#define NEC_BIT_LOW_MAX        1100

#define NEC_ZERO_HIGH_MIN      200
#define NEC_ZERO_HIGH_MAX      1150

#define NEC_ONE_HIGH_MIN       900
#define NEC_ONE_HIGH_MAX       3000

//======================================================
// makeNoIR()
//======================================================

IRResult makeNoIR()
{
  IRResult r;
  r.found = false;
  r.repeat = false;
  r.nec = false;
  r.bits = 0;
  r.code = 0;
  r.address = 0;
  r.command = 0;
  return r;
}

//======================================================
// inRange()
//======================================================

bool inRange(unsigned long v, unsigned long mn, unsigned long mx)
{
  return (v >= mn && v <= mx);
}

//======================================================
// captureIRRaw()
//
// ИК-приёмник обычно:
// idle = HIGH
// импульс = LOW
//
// raw[0] = длительность LOW стартового импульса
// raw[1] = длительность HIGH паузы
// raw[2] = следующий LOW
// ...
//======================================================

bool captureIRRaw(uint16_t *raw, uint8_t &count)
{
  count = 0;

  // Ничего нет — сразу выходим, чтобы меню/кнопки не тормозили.
  if (digitalRead(IR_PIN) != LOW)
    return false;

  uint8_t level = LOW;
  unsigned long lastEdge = micros();

  while (count < IR_RAW_MAX)
  {
    while (digitalRead(IR_PIN) == level)
    {
      unsigned long now = micros();

      // Длинная пауза = конец посылки.
      if (now - lastEdge > IR_GAP_US)
      {
        if (count > 4)
          return true;
        else
          return false;
      }
    }

    unsigned long now = micros();
    unsigned long dur = now - lastEdge;

    if (dur > 65535UL)
      dur = 65535UL;

    raw[count++] = (uint16_t)dur;

    level = !level;
    lastEdge = now;
  }

  return (count > 4);
}

//======================================================
// decodeNECFromRaw()
//======================================================

IRResult decodeNECFromRaw(uint16_t *raw, uint8_t count)
{
  IRResult r = makeNoIR();

  if (count < 4)
    return r;

  // NEC repeat: 9ms LOW + 2.25ms HIGH + короткий LOW
  if (inRange(raw[0], NEC_LEAD_LOW_MIN, NEC_LEAD_LOW_MAX) &&
      inRange(raw[1], NEC_REPEAT_HIGH_MIN, NEC_REPEAT_HIGH_MAX))
  {
    r.found = true;
    r.repeat = true;
    r.nec = true;
    return r;
  }

  if (count < 66)
    return r;

  if (!inRange(raw[0], NEC_LEAD_LOW_MIN, NEC_LEAD_LOW_MAX))
    return r;

  if (!inRange(raw[1], NEC_LEAD_HIGH_MIN, NEC_LEAD_HIGH_MAX))
    return r;

  uint32_t code = 0;

  for (uint8_t i = 0; i < 32; i++)
  {
    uint8_t lowIndex = 2 + i * 2;
    uint8_t highIndex = lowIndex + 1;

    uint16_t bitLow = raw[lowIndex];
    uint16_t bitHigh = raw[highIndex];

    if (!inRange(bitLow, NEC_BIT_LOW_MIN, NEC_BIT_LOW_MAX))
      return makeNoIR();

    if (inRange(bitHigh, NEC_ZERO_HIGH_MIN, NEC_ZERO_HIGH_MAX))
    {
      // bit 0
    }
    else if (inRange(bitHigh, NEC_ONE_HIGH_MIN, NEC_ONE_HIGH_MAX))
    {
      code |= (1UL << i); // NEC передаёт младший бит первым
    }
    else
    {
      return makeNoIR();
    }
  }

  r.found = true;
  r.repeat = false;
  r.nec = true;
  r.bits = 32;
  r.code = code;
  r.address = (uint8_t)(code & 0xFF);
  r.command = (uint8_t)((code >> 16) & 0xFF);

  return r;
}

//======================================================
// IR diagnostic helpers
//======================================================

String hexPad(uint32_t value, uint8_t digits)
{
  String s = String(value, HEX);
  s.toUpperCase();
  while (s.length() < digits) s = "0" + s;
  return s;
}

void drawIRValueLine(const char* label, String value, int y, uint16_t valueColor)
{
  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawString(label, 8, y, 1);
  tft.setTextColor(valueColor, COLOR_BG);
  tft.drawString(value, 58, y, 1);
}

//======================================================
// drawIRScreen()
//======================================================

void drawIRScreen()
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();

  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  drawLocalizedIRTitle(CENTER_X, 20, TFT_YELLOW);

  tft.setTextColor(TFT_WHITE, COLOR_BG);
  drawLocalizedIRPrompt(CENTER_X, 44, TFT_WHITE);

  tft.setTextColor(TFT_CYAN, COLOR_BG);
  tft.drawCentreString("IR DIAGNOSTIC", CENTER_X, 66, 2);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedHoldOKMenu(CENTER_X, 116, TFT_GREEN);
}

//======================================================
// drawNECResult()
//======================================================

void drawNECResult(IRResult r, uint32_t lastCode, uint16_t packetCount)
{
  tft.fillRect(1, 32, 158, 80, COLOR_BG);

  tft.drawRoundRect(5, 34, 150, 74, 5, TFT_DARKGREY);

  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawString("PROTOCOL:", 10, 39, 1);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  if (r.repeat)
    tft.drawString("NEC REPEAT", 76, 39, 1);
  else
    tft.drawString("NEC 32", 76, 39, 1);

  uint32_t shownCode = r.repeat ? lastCode : r.code;
  String codeText = "0x" + hexPad(shownCode, 8);

  tft.setTextColor(TFT_CYAN, COLOR_BG);
  tft.drawCentreString(codeText, CENTER_X, 53, 2);

  if (!r.repeat)
  {
    drawIRValueLine("ADDR:", "0x" + hexPad(r.address, 2), 74, TFT_WHITE);
    drawIRValueLine("CMD:",  "0x" + hexPad(r.command, 2) + " / " + String(r.command), 86, TFT_WHITE);
  }
  else
  {
    drawIRValueLine("ADDR:", "--", 74, TFT_WHITE);
    drawIRValueLine("CMD:",  "--", 86, TFT_WHITE);
  }

  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawString("SIGNAL OK", 10, 98, 1);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  tft.drawString("CNT:", 104, 98, 1);
  tft.drawString(String(packetCount), 132, 98, 1);
}

//======================================================
// drawRAWResult()
//======================================================

void drawRAWResult(uint16_t *raw, uint8_t count, uint16_t packetCount)
{
  tft.fillRect(1, 32, 158, 80, COLOR_BG);

  tft.drawRoundRect(5, 34, 150, 74, 5, TFT_DARKGREY);

  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawString("PROTOCOL:", 10, 39, 1);

  tft.setTextColor(TFT_ORANGE, COLOR_BG);
  tft.drawString("UNKNOWN RAW", 76, 39, 1);

  drawIRValueLine("PULSES:", String(count), 54, TFT_YELLOW);

  String line1 = "";
  String line2 = "";

  for (uint8_t i = 0; i < count && i < 4; i++)
  {
    line1 += String(raw[i]);
    if (i < 3) line1 += " ";
  }

  for (uint8_t i = 4; i < count && i < 8; i++)
  {
    line2 += String(raw[i]);
    if (i < 7) line2 += " ";
  }

  tft.setTextColor(TFT_CYAN, COLOR_BG);
  tft.drawString(line1, 10, 70, 1);
  tft.drawString(line2, 10, 82, 1);

  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawString("SIGNAL OK", 10, 98, 1);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  tft.drawString("CNT:", 104, 98, 1);
  tft.drawString(String(packetCount), 132, 98, 1);
}

//======================================================
// irRemoteLoop()
//======================================================

void irRemoteLoop()
{
  pinMode(IR_PIN, INPUT_PULLUP);

  drawIRScreen();

  uint16_t raw[IR_RAW_MAX];
  uint8_t rawCount = 0;

  uint32_t lastCode = 0;
  uint16_t packetCount = 0;
  unsigned long lastDraw = 0;
  unsigned long batteryTimer = 0;

  while (true)
  {
    if (autoOffTick())
    {
      drawIRScreen();
      lastDraw = 0;
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

    if (captureIRRaw(raw, rawCount))
    {
      packetCount++;
      IRResult r = decodeNECFromRaw(raw, rawCount);

      if (r.found && r.nec)
      {
        if (!r.repeat)
        {
          lastCode = r.code;
          soundFound();
        }

        drawNECResult(r, lastCode, packetCount);
      }
      else
      {
        drawRAWResult(raw, rawCount, packetCount);
      }

      lastDraw = millis();
    }

    if (millis() - lastDraw > 6000 && lastDraw != 0)
    {
      drawIRScreen();
      lastDraw = 0;
    }

    delay(2);
  }
}
