/****************************************************************
                YANA MULTITESTER
                File: Boot.ino

  Патриотическая заставка:
  - украинский флаг на весь экран;
  - затем крупными буквами появляется надпись:

        СЛАВА
        УКРАЇНІ

  Важно:
  стандартные шрифты TFT_eSPI часто не выводят кириллицу,
  поэтому буквы нарисованы вручную пиксельными матрицами.
****************************************************************/

#include "Config.h"

//======================================================
// Цвета заставки
//======================================================

#define UA_BLUE        0x055F
#define UA_YELLOW      0xFEC0
#define UA_WHITE       TFT_WHITE
#define UA_DARK        TFT_BLACK

// Выбранная заставка: 0...4. Значение сохраняется в Settings.ino.
uint8_t bootStyle = 0;

uint16_t bootBackgroundColor()
{
  switch (bootStyle)
  {
    case 1: return 0x084F; // космический пейзаж
    case 2: return 0x0210; // техно-мозг
    case 3: return 0x18E3; // синий фрактал
    case 4: return TFT_BLACK; // неоновые соты
    default: return UA_BLUE;
  }
}


static void fillVerticalGradient(int x, int y, int w, int h, uint16_t c1, uint16_t c2)
{
  uint8_t r1 = (c1 >> 11) & 0x1F;
  uint8_t g1 = (c1 >> 5) & 0x3F;
  uint8_t b1 = c1 & 0x1F;
  uint8_t r2 = (c2 >> 11) & 0x1F;
  uint8_t g2 = (c2 >> 5) & 0x3F;
  uint8_t b2 = c2 & 0x1F;

  for (int i = 0; i < h; i++)
  {
    uint8_t r = r1 + ((int)(r2 - r1) * i) / (h > 1 ? (h - 1) : 1);
    uint8_t g = g1 + ((int)(g2 - g1) * i) / (h > 1 ? (h - 1) : 1);
    uint8_t b = b1 + ((int)(b2 - b1) * i) / (h > 1 ? (h - 1) : 1);
    uint16_t c = (r << 11) | (g << 5) | b;
    tft.drawFastHLine(x, y + i, w, c);
  }
}

static void drawSimpleHex(int cx, int cy, int r, uint16_t color)
{
  int x1 = cx - r;
  int x2 = cx - r / 2;
  int x3 = cx + r / 2;
  int x4 = cx + r;
  int y1 = cy;
  int y2 = cy - r;
  int y3 = cy + r;

  tft.drawLine(x2, y2, x3, y2, color);
  tft.drawLine(x3, y2, x4, y1, color);
  tft.drawLine(x4, y1, x3, y3, color);
  tft.drawLine(x3, y3, x2, y3, color);
  tft.drawLine(x2, y3, x1, y1, color);
  tft.drawLine(x1, y1, x2, y2, color);
}

void bootCircuitBoard()
{
  // Заставка 2: радиодетали на электронной плате.
  const uint16_t bgTop      = 0x0108;
  const uint16_t bgBottom   = 0x02B0;
  const uint16_t traceColor = 0x2E9F;
  const uint16_t padColor   = 0xB71C;
  const uint16_t chipBody   = 0x18C3;
  const uint16_t chipPin    = 0xC638;
  const uint16_t whiteSilk  = TFT_WHITE;

  fillVerticalGradient(0, 0, LCD_WIDTH, LCD_HEIGHT, bgTop, bgBottom);

  // Фон платы: дорожки и контактные площадки.
  for (int y = 14; y <= 112; y += 18)
  {
    tft.drawFastHLine(0, y, 160, traceColor);
    for (int x = 12; x <= 148; x += 34)
      tft.fillCircle(x, y, 3, padColor);
  }
  for (int x = 16; x <= 144; x += 24)
  {
    tft.drawFastVLine(x, 0, 128, 0x2458);
    for (int y = 10; y <= 118; y += 36)
      tft.fillCircle(x, y, 2, 0x6F5F);
  }

  // Центральная микросхема.
  tft.fillRoundRect(53, 42, 54, 32, 4, chipBody);
  tft.drawRoundRect(53, 42, 54, 32, 4, 0x7DFF);
  for (int i = 0; i < 6; i++)
  {
    tft.fillRect(48, 46 + i * 4, 5, 2, chipPin);
    tft.fillRect(107, 46 + i * 4, 5, 2, chipPin);
  }
  tft.drawRect(64, 50, 10, 8, 0x5D7C);
  tft.drawRect(79, 50, 17, 16, 0x5D7C);
  tft.fillCircle(60, 48, 2, TFT_WHITE);

  // Дорожки к микросхеме.
  tft.drawLine(32, 26, 32, 58, traceColor);
  tft.drawLine(32, 58, 53, 58, traceColor);
  tft.fillCircle(32, 26, 3, padColor);

  tft.drawLine(128, 20, 128, 50, traceColor);
  tft.drawLine(107, 50, 128, 50, traceColor);
  tft.fillCircle(128, 20, 3, padColor);

  tft.drawLine(22, 96, 22, 72, traceColor);
  tft.drawLine(22, 72, 53, 72, traceColor);
  tft.fillCircle(22, 96, 3, padColor);

  tft.drawLine(138, 98, 138, 66, traceColor);
  tft.drawLine(107, 66, 138, 66, traceColor);
  tft.fillCircle(138, 98, 3, padColor);

  // Резистор (слева сверху).
  tft.drawLine(8, 34, 20, 34, 0xCE59);
  tft.drawLine(44, 34, 56, 34, 0xCE59);
  tft.fillRoundRect(20, 29, 24, 10, 3, 0xF6BA);
  tft.drawRoundRect(20, 29, 24, 10, 3, 0x7BCF);
  tft.drawFastVLine(25, 30, 8, TFT_RED);
  tft.drawFastVLine(29, 30, 8, TFT_ORANGE);
  tft.drawFastVLine(34, 30, 8, TFT_YELLOW);
  tft.drawFastVLine(39, 30, 8, TFT_BLUE);

  // Электролитический конденсатор (справа сверху).
  tft.drawLine(120, 30, 132, 30, 0xCE59);
  tft.drawLine(120, 42, 132, 42, 0xCE59);
  tft.fillRoundRect(132, 24, 15, 24, 3, TFT_BLUE);
  tft.drawRoundRect(132, 24, 15, 24, 3, 0xB71C);
  tft.drawFastVLine(139, 26, 20, TFT_WHITE);
  tft.drawFastHLine(135, 32, 5, TFT_WHITE);
  tft.drawFastVLine(138, 37, 6, TFT_WHITE);

  // Диод (слева снизу).
  tft.drawLine(10, 92, 22, 92, 0xCE59);
  tft.drawLine(40, 92, 52, 92, 0xCE59);
  tft.fillTriangle(22, 84, 22, 100, 34, 92, TFT_YELLOW);
  tft.drawTriangle(22, 84, 22, 100, 34, 92, TFT_ORANGE);
  tft.drawFastVLine(36, 84, 16, TFT_RED);

  // Катушка (справа снизу).
  tft.drawLine(108, 94, 114, 94, 0xCE59);
  for (int i = 0; i < 4; i++)
    tft.drawCircle(118 + i * 6, 94, 3, 0xFFE0);
  tft.drawLine(139, 94, 150, 94, 0xCE59);

  // Транзистор (центр снизу).
  tft.drawCircle(78, 100, 10, 0x7DFF);
  tft.drawLine(68, 100, 58, 100, 0x7DFF);
  tft.drawLine(83, 94, 92, 84, 0x7DFF);
  tft.drawLine(83, 106, 92, 116, 0x7DFF);
  tft.drawLine(72, 94, 84, 106, TFT_GREENYELLOW);
  tft.drawLine(81, 103, 84, 106, TFT_GREENYELLOW);
  tft.drawLine(84, 106, 81, 109, TFT_GREENYELLOW);

  // Небольшие декоративные подписи-шёлкография.
  tft.drawRect(6, 6, 148, 116, 0x044F);
  tft.drawFastHLine(60, 82, 40, whiteSilk);
  tft.drawFastVLine(80, 82, 18, whiteSilk);

  // Блики на плате.
  tft.drawLine(4, 8, 36, 8, 0xBFFF);
  tft.drawLine(124, 12, 154, 12, 0xBFFF);
  tft.drawLine(110, 116, 146, 116, 0xBFFF);
}

void bootOscilloscope()
{
  // Заставка 3: техно-мозг.
  fillVerticalGradient(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x0210, 0x0458);

  // Фоновые дорожки.
  for (int y = 14; y <= 112; y += 18)
  {
    tft.drawFastHLine(0, y, 48, 0x3DFF);
    tft.drawFastHLine(112, y, 48, 0x3DFF);
    tft.fillCircle(20, y, 2, TFT_WHITE);
    tft.fillCircle(140, y, 2, TFT_WHITE);
  }
  for (int x = 10; x <= 48; x += 14)
  {
    tft.drawFastVLine(x, 0, 28, 0x3DFF);
    tft.drawFastVLine(x, 100, 28, 0x3DFF);
  }
  for (int x = 112; x <= 150; x += 14)
  {
    tft.drawFastVLine(x, 0, 28, 0x3DFF);
    tft.drawFastVLine(x, 100, 28, 0x3DFF);
  }

  // Центральный ореол.
  tft.fillCircle(80, 64, 36, 0x039F);
  tft.drawCircle(80, 64, 38, 0xBFFF);
  tft.drawCircle(80, 64, 42, 0x7DFF);

  // Профиль головы (упрощённо).
  tft.drawCircle(80, 58, 22, TFT_WHITE);
  tft.drawLine(90, 79, 90, 101, TFT_WHITE);
  tft.drawLine(90, 101, 76, 116, TFT_WHITE);
  tft.drawLine(76, 116, 64, 96, TFT_WHITE);
  tft.drawLine(64, 96, 58, 76, TFT_WHITE);
  tft.drawLine(58, 76, 58, 46, TFT_WHITE);
  tft.drawLine(90, 58, 98, 62, TFT_WHITE);
  tft.drawLine(98, 62, 94, 68, TFT_WHITE);
  tft.drawLine(94, 68, 96, 74, TFT_WHITE);
  tft.drawLine(96, 74, 90, 78, TFT_WHITE);

  // Мозг внутри.
  tft.fillCircle(76, 56, 8, 0xFFE0);
  tft.fillCircle(84, 56, 8, 0xFD20);
  tft.fillCircle(72, 64, 7, 0xFEC0);
  tft.fillCircle(82, 66, 7, 0xFD20);
  tft.drawCircle(76, 56, 8, 0xA145);
  tft.drawCircle(84, 56, 8, 0xA145);
  tft.drawCircle(72, 64, 7, 0xA145);
  tft.drawCircle(82, 66, 7, 0xA145);

  // Связи-нейроны.
  const uint8_t nx[] = {63, 70, 88, 98, 74, 86, 80};
  const uint8_t ny[] = {48, 42, 44, 54, 74, 78, 86};
  for (int i = 0; i < 7; i++)
  {
    tft.fillCircle(nx[i], ny[i], 2, TFT_WHITE);
    tft.drawLine(80, 60, nx[i], ny[i], 0xAFE5);
  }

  // Дорожки к краям.
  for (int i = 0; i < 5; i++)
  {
    int yy = 28 + i * 14;
    tft.drawLine(42, yy, 58, yy, 0xAFE5);
    tft.drawLine(102, yy, 118, yy, 0xAFE5);
    tft.fillCircle(42, yy, 2, 0xFFE0);
    tft.fillCircle(118, yy, 2, 0xFFE0);
  }
}

void bootRobotYana()
{
  // Заставка 4: синий фрактал.
  fillVerticalGradient(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x18E3, 0x7D7F);

  // Крупные фрактальные пятна.
  tft.fillCircle(34, 32, 26, 0x5CFF);
  tft.fillCircle(24, 58, 18, 0x3BFF);
  tft.fillCircle(56, 64, 22, 0x42D8);
  tft.fillCircle(104, 36, 24, 0x4B9F);
  tft.fillCircle(124, 70, 28, 0x3A5F);
  tft.fillCircle(78, 96, 24, 0x6DFF);
  tft.fillCircle(140, 24, 16, 0x94BF);

  // Светлые прожилки.
  tft.drawLine(8, 24, 64, 78, TFT_WHITE);
  tft.drawLine(20, 10, 76, 62, 0xDFFF);
  tft.drawLine(40, 54, 104, 18, TFT_WHITE);
  tft.drawLine(62, 70, 132, 38, 0xDFFF);
  tft.drawLine(58, 92, 138, 82, TFT_WHITE);
  tft.drawLine(24, 118, 94, 70, 0xDFFF);
  tft.drawLine(100, 126, 148, 56, TFT_WHITE);

  // Мелкие ответвления.
  tft.drawLine(26, 42, 14, 56, TFT_WHITE);
  tft.drawLine(44, 38, 58, 22, TFT_WHITE);
  tft.drawLine(82, 28, 96, 12, TFT_WHITE);
  tft.drawLine(112, 56, 128, 46, TFT_WHITE);
  tft.drawLine(88, 88, 74, 108, TFT_WHITE);
  tft.drawLine(126, 94, 144, 110, TFT_WHITE);

  // Внутренние контуры.
  tft.drawCircle(34, 32, 12, 0xBFFF);
  tft.drawCircle(56, 64, 10, 0xBFFF);
  tft.drawCircle(104, 36, 12, 0xBFFF);
  tft.drawCircle(124, 70, 14, 0xBFFF);
  tft.drawCircle(78, 96, 10, 0xBFFF);
}

void bootLightning()
{
  // Заставка 5: неоновые соты.
  fillVerticalGradient(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x0000, 0x100A);
  for (int row = 0; row < 7; row++)
  {
    int cy = 10 + row * 18;
    int offset = (row & 1) ? 10 : 0;
    for (int col = 0; col < 9; col++)
    {
      int cx = 12 + offset + col * 18;
      uint16_t c = (col < 4) ? 0x07FF : 0xC81F;
      if ((row + col) & 1) c = (col < 4) ? 0x3DFF : 0xD5FF;
      drawSimpleHex(cx, cy, 7, c);
      if (((row + col) % 4) == 0) tft.fillCircle(cx, cy, 2, TFT_WHITE);
    }
  }

  // Неоновые лучи.
  for (int i = 0; i < 6; i++)
  {
    tft.drawLine(0, 18 + i * 3, 70, 42 + i * 2, 0x07FF);
    tft.drawLine(160, 20 + i * 3, 92, 50 + i * 2, 0xC81F);
    tft.drawLine(30 + i * 2, 128, 78, 86 - i * 2, 0x7DFF);
  }
}

//======================================================
// drawUkraineFlag()
//======================================================

void drawUkraineFlag()
{
  tft.fillScreen(UA_BLUE);
  tft.fillRect(0, LCD_HEIGHT / 2, LCD_WIDTH, LCD_HEIGHT / 2, UA_YELLOW);
}

//======================================================
// Пиксельные буквы 5x7
// 1 = пиксель буквы, 0 = пусто
//======================================================

const uint8_t GL_S[7] =
{
  B01111,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B01111
};

const uint8_t GL_L[7] =
{
  B00111,
  B01001,
  B01001,
  B01001,
  B01001,
  B01001,
  B11001
};

const uint8_t GL_A[7] =
{
  B01110,
  B10001,
  B10001,
  B11111,
  B10001,
  B10001,
  B10001
};

const uint8_t GL_V[7] =
{
  B11110,
  B10001,
  B10001,
  B11110,
  B10001,
  B10001,
  B11110
};

const uint8_t GL_U[7] =
{
  B10001,
  B10001,
  B10001,
  B01111,
  B00001,
  B10001,
  B01110
};

const uint8_t GL_K[7] =
{
  B10001,
  B10010,
  B10100,
  B11000,
  B10100,
  B10010,
  B10001
};

const uint8_t GL_R[7] =
{
  B11110,
  B10001,
  B10001,
  B11110,
  B10000,
  B10000,
  B10000
};

const uint8_t GL_YI[7] =
{
  B01010,
  B00000,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

const uint8_t GL_N[7] =
{
  B10001,
  B10001,
  B10001,
  B11111,
  B10001,
  B10001,
  B10001
};

const uint8_t GL_I[7] =
{
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

//======================================================
// drawGlyph5x7()
//======================================================

void drawGlyph5x7(const uint8_t glyph[7], int x, int y, uint8_t scale, uint16_t color)
{
  for (int row = 0; row < 7; row++)
  {
    for (int col = 0; col < 5; col++)
    {
      if (glyph[row] & (1 << (4 - col)))
      {
        tft.fillRect(x + col * scale, y + row * scale, scale, scale, color);
      }
    }
  }
}

//======================================================
// drawWordSLAVA()
//======================================================

void drawWordSLAVA(uint16_t color, int y, uint8_t scale)
{
  const int glyphW = 5 * scale;
  const int gap = scale;
  const int wordW = 5 * glyphW + 4 * gap;
  int x = (LCD_WIDTH - wordW) / 2;

  drawGlyph5x7(GL_S, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_L, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_A, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_V, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_A, x, y, scale, color);
}

//======================================================
// drawWordUKRAINI()
//======================================================

void drawWordUKRAINI(uint16_t color, int y, uint8_t scale)
{
  const int glyphW = 5 * scale;
  const int gap = scale;
  const int wordW = 7 * glyphW + 6 * gap;
  int x = (LCD_WIDTH - wordW) / 2;

  drawGlyph5x7(GL_U, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_K, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_R, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_A, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_YI, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_N, x, y, scale, color);
  x += glyphW + gap;

  drawGlyph5x7(GL_I, x, y, scale, color);
}


//======================================================
// animateSlogan()
//======================================================

void animateSlogan()
{
  // Флаг уже показан. Делаем появление текста.
  delay(700);

  // Сначала СЛАВА
  drawWordSLAVA(UA_DARK, 23, 3);
  drawWordSLAVA(UA_WHITE, 20, 3);
  delay(650);

  // Потом УКРАЇНІ
  drawWordUKRAINI(UA_DARK, 78, 3);
  drawWordUKRAINI(UA_BLUE, 75, 3);
  delay(650);
}



//======================================================
// Бегущая строка на заставке
//======================================================

const char* BOOT_SCROLL_TEXT = "Slava Ukraine   proekt sozdan Gemini Yana i Udjen superstar     ** genia.1980 @ gmail.com ** ";
const char* BOOT_SCROLL_PAUSE_TEXT = "* genia.1980 @ gmail.com *";
int bootScrollX = LCD_WIDTH;
int bootScrollTextWidth = 0;
int bootScrollPauseX = -240;
unsigned long bootScrollTimer = 0;
unsigned long bootScrollPauseTimer = 0;
bool bootScrollPaused = false;
bool bootScrollPausedThisPass = false;

void bootMarqueeInit()
{
  bootScrollX = LCD_WIDTH;
  bootScrollTextWidth = strlen(BOOT_SCROLL_TEXT) * 6;
  bootScrollTimer = 0;
  bootScrollPauseTimer = 0;
  bootScrollPaused = false;
  bootScrollPausedThisPass = false;

  const char* match = strstr(BOOT_SCROLL_TEXT, BOOT_SCROLL_PAUSE_TEXT);
  if (match != NULL)
  {
    int pauseOffsetPx = (match - BOOT_SCROLL_TEXT) * 6;
    int pauseTextWidth = strlen(BOOT_SCROLL_PAUSE_TEXT) * 6;

    // Останавливаем участок "* genia.1980 @ gmail.com *" примерно по центру экрана.
    bootScrollPauseX = ((LCD_WIDTH - pauseTextWidth) / 2) - pauseOffsetPx;
  }
  else
  {
    bootScrollPauseX = -240;
  }

  tft.fillRect(0, LCD_HEIGHT - 11, LCD_WIDTH, 11, TFT_BLACK);
}

void bootMarqueeDraw()
{
  tft.fillRect(0, LCD_HEIGHT - 11, LCD_WIDTH, 11, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(BOOT_SCROLL_TEXT, bootScrollX, LCD_HEIGHT - 10, 1);
}

void bootMarqueeTick()
{
  if (bootScrollPaused)
  {
    if (millis() - bootScrollPauseTimer >= 3000)
    {
      bootScrollPaused = false;
      bootScrollPausedThisPass = true;
    }
    return;
  }

  if (millis() - bootScrollTimer < 30) return;
  bootScrollTimer = millis();

  bootMarqueeDraw();

  if (!bootScrollPausedThisPass && bootScrollX <= bootScrollPauseX)
  {
    bootScrollX = bootScrollPauseX;
    bootMarqueeDraw();
    bootScrollPaused = true;
    bootScrollPauseTimer = millis();
    return;
  }

  bootScrollX--;
  if (bootScrollX < -bootScrollTextWidth)
  {
    bootScrollX = LCD_WIDTH;
    bootScrollPaused = false;
    bootScrollPausedThisPass = false;
  }
}

//======================================================
// bootAnimation()
//======================================================

void bootAnimation()
{
  clearScreen();
  delay(120);

  switch (bootStyle)
  {
    case 1: bootCircuitBoard(); break;
    case 2: bootOscilloscope(); break;
    case 3: bootRobotYana(); break;
    case 4: bootLightning(); break;
    default:
      drawUkraineFlag();
      delay(700);
      animateSlogan();
      break;
  }

  soundStartupMelody();
  delay(1100);
}
