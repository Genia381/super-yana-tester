/****************************************************************
                SUPER YANA TESTER
                File: NanoUART.ino

  Приём результатов измерительной головы Arduino Nano.

  Nano D1/TX -> делитель 1k/2k -> Pico GP1/RX
  Общий GND обязателен.

  Serial1 Pico:
  GP0 = TX (зарезервирован на будущее)
  GP1 = RX
  9600 бод
****************************************************************/

#include "Config.h"

#define NANO_UART Serial1
#define NANO_UART_BAUD 9600
// Nano может долго измерять большие конденсаторы и в это время молчать.
// Ждём до 90 секунд после @BEGIN.
#define NANO_PACKET_TIMEOUT 90000UL
#define NANO_MAX_LINES 9
#define NANO_MAX_LINE_LEN 38

static String nanoRxLine;
static String nanoResultLines[NANO_MAX_LINES];
static uint8_t nanoResultLineCount = 0;
static bool nanoPacketActive = false;
static unsigned long nanoMeasureStartTime = 0;
static bool nanoSearchScreenActive = false;
static bool nanoCommandPending = false;
static unsigned long nanoLastCommandTime = 0;
static uint8_t nanoCommandRetries = 0;

void nanoUartInit()
{
  // В Arduino Mbed RP2040 Serial1 использует GP0/GP1.
  NANO_UART.begin(NANO_UART_BAUD);
  nanoRxLine.reserve(64);
}

void nanoClearResult()
{
  nanoResultLineCount = 0;
  for (uint8_t i = 0; i < NANO_MAX_LINES; i++)
    nanoResultLines[i] = "";
}

static String nanoCleanLine(String s)
{
  s.trim();

  // Текстовые замены для специальных символов тестера Nano.
  s.replace("Ohm", "Ohm");
  return s;
}

static void nanoStoreLine(const String &line)
{
  if (line.length() == 0)
    return;

  // Если строка длинная, переносим её на несколько строк.
  uint16_t pos = 0;
  while (pos < line.length() && nanoResultLineCount < NANO_MAX_LINES)
  {
    String part = line.substring(pos, pos + NANO_MAX_LINE_LEN);
    nanoResultLines[nanoResultLineCount++] = part;
    pos += NANO_MAX_LINE_LEN;
  }
}

static void nanoDrawWaiting()
{
  nanoSearchScreenActive = false;
  drawSearchNoComponentScreen();
}

static void nanoDrawMeasuring()
{
  nanoSearchScreenActive = true;
  drawSearchingRobot(true);
}

void nanoDrawResult()
{
  tft.fillRect(2, 20, 156, 90, COLOR_BG);

  if (nanoResultLineCount == 0)
  {
    tft.setTextColor(TFT_RED, COLOR_BG);
    tft.drawCentreString("NO DATA", CENTER_X, 56, 2);
    return;
  }

  // Умещаем до девяти строк шрифтом 1.
  tft.setTextColor(TFT_GREEN, COLOR_BG);
  tft.drawCentreString("NANO RESULT", CENTER_X, 22, 1);

  int y = 33;
  for (uint8_t i = 0; i < nanoResultLineCount; i++)
  {
    uint16_t color = (i == 0) ? TFT_CYAN : TFT_WHITE;
    tft.setTextColor(color, COLOR_BG);
    tft.drawString(nanoResultLines[i], 6, y, 1);
    y += 9;
    if (y > 105)
      break;
  }
}


static uint8_t nanoPinFromDigit(char c)
{
  if (c == '1') return TP1;
  if (c == '2') return TP2;
  return TP3;
}

static bool nanoFindPinsAroundSymbol(const String &s, const String &symbol,
                                     uint8_t &a, uint8_t &b)
{
  int p = s.indexOf(symbol);
  if (p < 0)
    return false;

  // Ищем номер щупа 1..3 непосредственно слева от символа.
  int left = p - 1;
  while (left >= 0)
  {
    char c = s[left];
    if (c >= '1' && c <= '3')
    {
      a = nanoPinFromDigit(c);
      break;
    }
    // Не уходим в предыдущую строку или в числовое значение.
    if (c == '\n' || c == '\r')
      break;
    left--;
  }

  // Ищем номер щупа 1..3 непосредственно справа от символа.
  int right = p + symbol.length();
  while (right < (int)s.length())
  {
    char c = s[right];
    if (c >= '1' && c <= '3')
    {
      b = nanoPinFromDigit(c);
      break;
    }
    if (c == '\n' || c == '\r')
      break;
    right++;
  }

  return (left >= 0 && right < (int)s.length());
}

static bool nanoFindTwoPins(const String &s, uint8_t &a, uint8_t &b)
{
  a = TP1;
  b = TP2;

  // Двухвыводные детали Nano печатает так:
  // 1-[=]-2, 1-||-2, 1->|-2 или 1-|<-2.
  if (nanoFindPinsAroundSymbol(s, "[=]", a, b)) return true;
  if (nanoFindPinsAroundSymbol(s, "||",  a, b)) return true;
  if (nanoFindPinsAroundSymbol(s, ">|",  a, b)) return true;
  if (nanoFindPinsAroundSymbol(s, "|<",  a, b)) return true;

  return false;
}


static bool nanoFindDiodePins(const String &s, uint8_t &anode, uint8_t &cathode)
{
  uint8_t leftPin = TP1;
  uint8_t rightPin = TP2;

  // Стрелка вправо: слева анод, справа катод.
  if (nanoFindPinsAroundSymbol(s, ">|", leftPin, rightPin))
  {
    anode = leftPin;
    cathode = rightPin;
    return true;
  }

  // Стрелка влево: слева катод, справа анод.
  // Рисунок Pico всегда ориентирован анодом влево и катодом вправо,
  // поэтому номера щупов нужно поменять местами.
  if (nanoFindPinsAroundSymbol(s, "|<", leftPin, rightPin))
  {
    anode = rightPin;
    cathode = leftPin;
    return true;
  }

  return false;
}

static float nanoParseSIAt(const String &s, int start, char wantedUnit)
{
  if (start < 0) return 0.0f;
  while (start < (int)s.length() && !(isDigit(s[start]) || s[start] == '.' || s[start] == '-')) start++;
  if (start >= (int)s.length()) return 0.0f;

  int end = start;
  while (end < (int)s.length() && (isDigit(s[end]) || s[end] == '.' || s[end] == '-')) end++;
  float value = s.substring(start, end).toFloat();

  while (end < (int)s.length() && s[end] == ' ') end++;
  float mul = 1.0f;
  if (end < (int)s.length())
  {
    char p = s[end];
    if (p == 'p') mul = 1e-12f;
    else if (p == 'n') mul = 1e-9f;
    else if (p == 'u') mul = 1e-6f;
    else if (p == 'm') mul = 1e-3f;
    else if (p == 'k') mul = 1e3f;
    else if (p == 'M') mul = 1e6f;
  }

  // Возвращаем в единицах структуры Pico.
  if (wantedUnit == 'F') return value * mul * 1e6f;  // uF
  if (wantedUnit == 'H') return value * mul * 1e3f;  // mH
  return value * mul;                                 // Ohm / V / безразмерное
}

static float nanoValueAfter(const String &all, const String &key, char unit)
{
  int p = all.indexOf(key);
  if (p < 0) return 0.0f;
  return nanoParseSIAt(all, p + key.length(), unit);
}

static bool nanoParsePinLayout(const String &all, char role, uint8_t &pin)
{
  int p = all.indexOf("123=");
  if (p < 0 || p + 7 > (int)all.length()) return false;
  String layout = all.substring(p + 4, p + 7);
  int idx = layout.indexOf(role);
  if (idx < 0) return false;
  pin = (idx == 0) ? TP1 : (idx == 1 ? TP2 : TP3);
  return true;
}

static bool nanoParseTriacPair(const String &all,
                               uint8_t &pairPin1,
                               uint8_t &pairPin2,
                               uint8_t &mainPin)
{
  const String pairKey = "TRIAC_PAIR=";
  const String mainKey = "TRIAC_MAIN=";
  int pairPos = all.indexOf(pairKey);
  if (pairPos < 0) return false;

  pairPos += pairKey.length();
  if (pairPos >= (int)all.length() || all[pairPos] == '?') return false;
  if (pairPos + 1 >= (int)all.length()) return false;

  char parsedPair1 = all[pairPos];
  char parsedPair2 = all[pairPos + 1];
  bool validPair = parsedPair1 >= '1' && parsedPair1 <= '3' &&
                   parsedPair2 >= '1' && parsedPair2 <= '3' &&
                   parsedPair1 != parsedPair2;
  if (!validPair) return false;

  int mainPos = all.indexOf(mainKey);
  if (mainPos < 0) return false;
  mainPos += mainKey.length();
  if (mainPos >= (int)all.length()) return false;

  char parsedMain = all[mainPos];
  bool validMain = parsedMain >= '1' && parsedMain <= '3' &&
                   parsedMain != parsedPair1 && parsedMain != parsedPair2;
  if (!validMain) return false;

  uint8_t pinSum = (parsedPair1 - '0') +
                   (parsedPair2 - '0') +
                   (parsedMain - '0');
  if (pinSum != 6) return false;

  pairPin1 = parsedPair1 - '0';
  pairPin2 = parsedPair2 - '0';
  mainPin = parsedMain - '0';
  return true;
}

static ComponentResult nanoParseComponent()
{
  ComponentResult c = {};
  c.type = COMP_UNKNOWN;
  c.pinA = TP1; c.pinB = TP2; c.pinC = TP3;
  c.triacPairKnown = false;

  String all;
  for (uint8_t i = 0; i < nanoResultLineCount; i++)
  {
    all += nanoResultLines[i];
    all += " ";
  }

  String upper = all;
  upper.toUpperCase();

  if (upper.indexOf("NO, UNKNOWN") >= 0 || upper.indexOf("DAMAGED PART") >= 0)
  {
    c.type = COMP_NONE;
    return c;
  }

  if (upper.indexOf("NPN") >= 0 || upper.indexOf("PNP") >= 0)
  {
    c.type = upper.indexOf("NPN") >= 0 ? COMP_NPN : COMP_PNP;
    nanoParsePinLayout(upper, 'B', c.pinA);
    nanoParsePinLayout(upper, 'C', c.pinB);
    nanoParsePinLayout(upper, 'E', c.pinC);
    c.hFE = nanoValueAfter(all, "B=", 0);
    c.voltage = nanoValueAfter(all, "Uf=", 'V');
    return c;
  }

  if (upper.indexOf("MOS") >= 0)
  {
    c.type = upper.indexOf("P-") >= 0 ? COMP_PMOS : COMP_NMOS;
    nanoParsePinLayout(upper, 'G', c.pinA);
    nanoParsePinLayout(upper, 'D', c.pinB);
    nanoParsePinLayout(upper, 'S', c.pinC);
    c.gateThreshold = nanoValueAfter(all, "Vt=", 'V');
    if (c.gateThreshold <= 0.0f) c.gateThreshold = nanoValueAfter(all, "@Vgs=", 'V');
    return c;
  }

  if (upper.indexOf("JFET") >= 0)
  {
    c.type = upper.indexOf("P-") >= 0 ? COMP_JFET_P : COMP_JFET_N;
    return c;
  }

  if (upper.indexOf("THYRISTOR") >= 0) { c.type = COMP_THYRISTOR; return c; }
  if (upper.indexOf("TRIAC") >= 0)
  {
    c.type = COMP_TRIAC;
    c.triacPairKnown = nanoParseTriacPair(upper,
                                          c.triacPairPin1,
                                          c.triacPairPin2,
                                          c.triacMainPin);
    return c;
  }

  // Одна строка диода содержит >| или |< и Uf=.
  if ((all.indexOf(">|") >= 0 || all.indexOf("|<") >= 0) && all.indexOf("Uf=") >= 0)
  {
    c.type = COMP_DIODE;
    nanoFindDiodePins(all, c.pinA, c.pinB);
    c.voltage = nanoValueAfter(all, "Uf=", 'V');
    if (c.voltage > 1.5f) c.type = COMP_LED;
    return c;
  }

  // Конденсатор: символ || и значение с F.
  if (all.indexOf("||") >= 0 && upper.indexOf("F") >= 0)
  {
    c.type = COMP_CAPACITOR;
    nanoFindTwoPins(all, c.pinA, c.pinB);
    c.capacitance = nanoParseSIAt(all, all.indexOf("||") + 2, 'F');
    if (c.capacitance <= 0.0f)
    {
      for (uint8_t i = 0; i < nanoResultLineCount; i++)
        if (nanoResultLines[i].indexOf('F') >= 0) c.capacitance = nanoParseSIAt(nanoResultLines[i], 0, 'F');
    }
    c.esr = nanoValueAfter(all, "ESR=", 0);
    c.vloss = nanoValueAfter(all, "Vloss=", 0);
    if (c.vloss <= 0.0f) c.vloss = nanoValueAfter(all, "VLOSS=", 0);
    return c;
  }

  // Резистор, возможно с измеренной индуктивностью.
  if (all.indexOf("[=]") >= 0 || upper.indexOf("OHM") >= 0)
  {
    nanoFindTwoPins(all, c.pinA, c.pinB);
    if (all.indexOf("L=") >= 0)
    {
      c.type = COMP_INDUCTOR;
      c.inductance = nanoValueAfter(all, "L=", 'H');

      // Катушка в отчёте Nano выводится как резистор с добавленной строкой L=.
      // Сохраняем также активное сопротивление обмотки, например .7Ohm.
      for (uint8_t i = 0; i < nanoResultLineCount; i++)
      {
        String u = nanoResultLines[i];
        u.toUpperCase();
        if (u.indexOf("OHM") >= 0)
        {
          c.resistance = nanoParseSIAt(nanoResultLines[i], 0, 'R');
          break;
        }
      }

      return c;
    }

    c.type = COMP_RESISTOR;
    for (uint8_t i = 0; i < nanoResultLineCount; i++)
    {
      String u = nanoResultLines[i]; u.toUpperCase();
      if (u.indexOf("OHM") >= 0)
      {
        c.resistance = nanoParseSIAt(nanoResultLines[i], 0, 'R');
        break;
      }
    }
    return c;
  }

  return c;
}

static ComponentType nanoDrawParsedOrText()
{
  ComponentResult c = nanoParseComponent();
  if (c.type == COMP_UNKNOWN || c.type == COMP_JFET_N || c.type == COMP_JFET_P ||
      c.type == COMP_THYRISTOR)
  {
    nanoDrawResult();
    return c.type;
  }

  // TRIAC теперь рисуется штатной функцией drawTRIACComponent(),
  // а не старым текстовым экраном.
  drawComponent(c);
  return c.type;
}

static void nanoProcessLine(String line)
{
  line = nanoCleanLine(line);
  if (line.length() == 0)
    return;

  // Дублируем принятые строки в USB Serial Pico для диагностики.
  Serial.print("NANO > ");
  Serial.println(line);

  if (line == "@BOOT,NANO_TESTER_HEAD,1")
  {
    if (!nanoPacketActive)
      nanoDrawWaiting();
    return;
  }

  if (line == "@BEGIN")
  {
    nanoCommandPending = false;
    nanoCommandRetries = 0;
    nanoPacketActive = true;
    nanoMeasureStartTime = millis();
    nanoClearResult();

    nanoSearchScreenActive = true;
    nanoDrawMeasuring();
    return;
  }

  if (line == "@END")
  {
    nanoPacketActive = false;
    nanoCommandPending = false;
    nanoCommandRetries = 0;
    nanoSearchScreenActive = false;

    ComponentType resultType = nanoDrawParsedOrText();

    if (resultType != COMP_NONE)
      soundFound();

    return;
  }

  if (nanoPacketActive)
    nanoStoreLine(line);
}


static void nanoSendTestCommand()
{
  NANO_UART.println("@TEST");
  NANO_UART.flush();
  nanoLastCommandTime = millis();
}

static void nanoStartMeasurement()
{
  if (nanoPacketActive)
    return;

  nanoRxLine = "";
  nanoClearResult();

  nanoCommandPending = true;
  nanoCommandRetries = 1;
  nanoSendTestCommand();

  nanoSearchScreenActive = true;
  drawSearchingRobot(false);
}

void nanoResetAfterWake()
{
  // При обычном авто-сне Nano и UART не выключаются.
  // Поэтому Serial1 нельзя завершать и запускать повторно:
  // на Arduino Mbed RP2040 после Serial1.end() повторный begin()
  // может не восстановить передачу на GP0/GP1.

  // Сбрасываем только внутреннее состояние незавершённого измерения.
  nanoPacketActive = false;
  nanoCommandPending = false;
  nanoCommandRetries = 0;
  nanoSearchScreenActive = false;
  nanoRxLine = "";
  nanoClearResult();

  // UART остаётся запущенным всё время. Удаляем только старые байты,
  // которые могли накопиться, пока подсветка была выключена.
  while (NANO_UART.available())
    NANO_UART.read();

  NANO_UART.flush();
}

void nanoRequestMeasurement()
{
  if (nanoPacketActive)
  {
    soundError();
    return;
  }

  nanoStartMeasurement();
}

void nanoUartPoll()
{
  while (NANO_UART.available())
  {
    char c = (char)NANO_UART.read();

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      if (nanoRxLine.length() > 0)
      {
        nanoProcessLine(nanoRxLine);
        nanoRxLine = "";
      }
    }
    else
    {
      if (nanoRxLine.length() < 120)
        nanoRxLine += c;
      else
        nanoRxLine = "";
    }
  }

  // Если из-за помех команда потерялась, повторяем @TEST
  // до трёх раз, пока Nano не ответит @BEGIN.
  if (nanoCommandPending && !nanoPacketActive)
  {
    if (millis() - nanoLastCommandTime >= 800UL)
    {
      if (nanoCommandRetries < 3)
      {
        nanoCommandRetries++;
        nanoSendTestCommand();
      }
      else
      {
        nanoCommandPending = false;
        nanoCommandRetries = 0;
        nanoSearchScreenActive = false;
        drawSearchNoComponentScreen();
      }
    }
  }

  if (nanoSearchScreenActive)
  {
    animateSearchingRobot();
  }


  if (nanoPacketActive)
  {
    // Настоящей ошибкой считаем только отсутствие @END более 90 секунд.
    unsigned long elapsed = millis() - nanoMeasureStartTime;
    if (elapsed > NANO_PACKET_TIMEOUT)
    {
      nanoPacketActive = false;
      nanoSearchScreenActive = false;
      tft.fillRect(2, 20, 156, 90, COLOR_BG);
      tft.setTextColor(TFT_RED, COLOR_BG);
      tft.drawCentreString("UART TIMEOUT", CENTER_X, 55, 2);
      soundError();
    }
  }
}

void nanoTesterScreenInit()
{
  nanoRxLine = "";
  nanoPacketActive = false;
  nanoSearchScreenActive = false;
  nanoCommandPending = false;
  nanoCommandRetries = 0;
  nanoClearResult();
}
