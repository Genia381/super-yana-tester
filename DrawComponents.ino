/****************************************************************
                YANA MULTITESTER
                File: DrawComponents.ino

    Все рисунки компонентов.

    Здесь только графика.
    Никакой логики определения компонентов нет.

****************************************************************/

#include "Config.h"

//======================================================
// ОБЩИЕ ЦВЕТА
//======================================================

// Цвет щупа №1
#define PIN1_COLOR     TFT_BLUE

// Цвет щупа №2
#define PIN2_COLOR     TFT_RED

// Цвет щупа №3
#define PIN3_COLOR     TFT_GREEN

//======================================================
// pinColor()
//
// Возвращает цвет выбранного щупа.
//======================================================

uint16_t pinColor(uint8_t pin)
{
    if (pin == TP1) return PIN1_COLOR;
    if (pin == TP2) return PIN2_COLOR;

    return PIN3_COLOR;
}

//======================================================
// pinName()
//
// Возвращает строку:
//
// "1"
// "2"
// "3"
//
//======================================================

const char* pinName(uint8_t pin)
{
    if (pin == TP1) return "1";
    if (pin == TP2) return "2";

    return "3";
}

//======================================================
// resistorBodyColor()
//
// Цвет корпуса резистора:
//
// Ohm  = жёлтый
// kOhm = голубой
// MOhm = красный
//======================================================

uint16_t resistorBodyColor(float r)
{
  if (r < 1000.0) return TFT_YELLOW;
  if (r < 1000000.0) return TFT_CYAN;
  return TFT_RED;
}

//======================================================
// drawResistorComponent()
//
// Рисует резистор.
// pinA, pinB  - номера щупов
// resistance - сопротивление
//======================================================

void drawResistorComponent(uint8_t pinA, uint8_t pinB, float resistance)
{
  int y = RESISTOR_Y;

  uint16_t bodyColor = resistorBodyColor(resistance);

  // Номера щупов
  tft.setTextColor(pinColor(pinA), COLOR_BG);
  tft.drawString(pinName(pinA), 22, y - 12, 4);

  tft.setTextColor(pinColor(pinB), COLOR_BG);
  tft.drawString(pinName(pinB), 132, y - 12, 4);

  // Провода
  tft.drawFastHLine(39, y, 19, TFT_CYAN);
  tft.drawFastHLine(102, y, 19, TFT_CYAN);

  // Корпус резистора
  tft.drawRoundRect(58, y - 11, 44, 22, 4, bodyColor);
  tft.drawRoundRect(59, y - 10, 42, 20, 3, bodyColor);

  // Полоски
  tft.fillRect(69, y - 7, 4, 14, TFT_GREEN);
  tft.fillRect(78, y - 7, 4, 14, TFT_RED);
  tft.fillRect(87, y - 7, 4, 14, TFT_BLUE);
  //tft.fillRect(96, y - 7, 4, 14, TFT_YELLOW);
}

//======================================================
// drawDiodeComponent()
//
// Рисует диод:
//
// 1 ----|>|---- 2
//
// pinA - анод
// pinB - катод
// voltage - падение напряжения на диоде
//======================================================

uint16_t ledTriangleColor(float uf)
{
  // Примерное определение цвета светодиода по прямому падению напряжения.
  // Только 5 цветов: красный, жёлтый, зелёный, синий, белый.
  if (uf < 1.95f) return TFT_RED;
  if (uf < 2.15f) return TFT_YELLOW;
  if (uf < 2.55f) return TFT_GREEN;
  if (uf < 3.20f) return TFT_BLUE;
  return TFT_WHITE;
}

void drawDiodeComponent(uint8_t pinA, uint8_t pinB, float voltage, uint16_t triangleColor)
{
  int y = RESISTOR_Y;

  // ---------------- Номера щупов ----------------

  tft.setTextColor(pinColor(pinA), COLOR_BG);
  tft.drawString(pinName(pinA), 22, y - 12, 4);

  tft.setTextColor(pinColor(pinB), COLOR_BG);
  tft.drawString(pinName(pinB), 132, y - 12, 4);

  // ---------------- Провода ----------------

  tft.drawFastHLine(39, y, 26, TFT_CYAN);
  tft.drawFastHLine(96, y, 25, TFT_CYAN);

  // ---------------- Символ диода ----------------
  // Треугольник показывает направление тока

  tft.fillTriangle(65, y - 14, 65, y + 14, 88, y, triangleColor);

  // Вертикальная черта катода
  tft.drawFastVLine(91, y - 15, 30, TFT_RED);
  tft.drawFastVLine(92, y - 15, 30, TFT_RED);

  // ---------------- Значение Uf ----------------

  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawCentreString("Uf = " + String(voltage, 3) + " V", CENTER_X, VALUE_Y, 2);
}

//======================================================
// drawCapacitorComponent()
//
// Рисует конденсатор:
//
// 1 ----| |---- 2
//
// pinA       - первый щуп
// pinB       - второй щуп
// capacitance - ёмкость
//======================================================

void drawCapacitorComponent(uint8_t pinA, uint8_t pinB, float capacitance, float esr, float vloss)
{
  int y = RESISTOR_Y;

  // pinA заряжается через 470 кОм и считается плюсом,
  // pinB сидит на земле и считается минусом.
  tft.setTextColor(pinColor(pinA), COLOR_BG);
  tft.drawString(pinName(pinA), 22, y - 12, 4);

  tft.setTextColor(pinColor(pinB), COLOR_BG);
  tft.drawString(pinName(pinB), 132, y - 12, 4);

  // Провода
  tft.drawFastHLine(39, y, 30, TFT_CYAN);
  tft.drawFastHLine(91, y, 30, TFT_CYAN);

  // Пластины неполярного обозначения конденсатора
  tft.drawFastVLine(72, y - 17, 34, TFT_YELLOW);
  tft.drawFastVLine(73, y - 17, 34, TFT_YELLOW);

  tft.drawFastVLine(87, y - 17, 34, TFT_YELLOW);
  tft.drawFastVLine(88, y - 17, 34, TFT_YELLOW);


  // Ёмкость, ESR и потеря напряжения
  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawCentreString(formatCapacitance(capacitance), CENTER_X, 84, 2);

  if (esr > 0.0f || vloss > 0.0f)
  {
    String esrValue = String(esr, 2);
    if (esrValue.startsWith("0."))
      esrValue.remove(0, 1);

    String infoText = String("ESR=") + esrValue + " Ohm  Vloss=" + String(vloss, 1) + "%";
    tft.drawCentreString(infoText, CENTER_X, 101, 1);
  }
}

//======================================================
// drawMOSFETComponent()
//
// c.pinA = Gate
// c.pinB = Drain
// c.pinC = Source
//======================================================

void drawMOSFETComponent(ComponentResult c)
{
  int cy = RESISTOR_Y - 1;

  uint16_t gateColor   = pinColor(c.pinA);
  uint16_t drainColor  = pinColor(c.pinB);
  uint16_t sourceColor = pinColor(c.pinC);

  tft.setTextColor(gateColor, COLOR_BG);
  tft.drawString(pinName(c.pinA), 22, cy - 7, 2);

  tft.setTextColor(drainColor, COLOR_BG);
  tft.drawString(pinName(c.pinB), 128, cy - 30, 2);

  tft.setTextColor(sourceColor, COLOR_BG);
  tft.drawString(pinName(c.pinC), 128, cy + 16, 2);

  tft.drawCircle(80, cy, 22, TFT_CYAN);

  // Gate
  tft.drawFastHLine(38, cy, 29, TFT_CYAN);
  tft.drawFastVLine(67, cy - 14, 28, TFT_WHITE);

  // Линии возле затвора
  tft.drawFastVLine(72, cy - 17, 8, TFT_WHITE);
  tft.drawFastVLine(72, cy + 9, 8, TFT_WHITE);
  tft.drawFastHLine(72, cy - 16, 18, TFT_WHITE);
  tft.drawFastHLine(72, cy + 15, 18, TFT_WHITE);
  tft.drawFastVLine(72, cy - 6, 11, TFT_WHITE);
  tft.drawFastHLine(72, cy - 1 , 8, TFT_WHITE);
  tft.drawFastVLine(80, cy - 1, 16, TFT_WHITE);

  // Канал
  tft.drawFastVLine(91, cy - 15, 11, TFT_WHITE);
  tft.drawFastVLine(91, cy + 5, 11, TFT_WHITE);

  // Drain / Source
  tft.drawLine(91, cy - 16, 108, cy - 22, TFT_CYAN);
  tft.drawFastHLine(108, cy - 22, 12, TFT_CYAN);
  tft.drawLine(91, cy + 16, 108, cy + 22, TFT_CYAN);
  tft.drawFastHLine(108, cy + 22, 12, TFT_CYAN);

  // Встроенный body diode и стрелка
  if (c.type == COMP_NMOS)
  {
    // N-канальный MOSFET
    tft.fillTriangle(87, cy + 4, 95, cy + 4, 91, cy - 5 , TFT_YELLOW);
    tft.drawFastHLine(87, cy - 6, 9, TFT_RED);
    tft.fillTriangle(77, cy + 10, 80, cy + 4, 83, cy + 10 , TFT_BLUE); 
  }
  else
  {
    // P-канальный MOSFET
    tft.fillTriangle(87, cy - 6, 95, cy - 6, 91, cy + 3, TFT_YELLOW);
    tft.drawFastHLine(87, cy + 4, 9, TFT_RED);
    tft.fillTriangle(77, cy + 4, 80, cy + 10, 83, cy + 4 , TFT_BLUE);
  }

  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawString("G", 42, cy - 12, 1);
  tft.drawString("D", 103, cy - 30, 1);
  tft.drawString("S", 104, cy + 24, 1);

  if (c.gateThreshold > 0.01)
    tft.drawCentreString("Vg=" + String(c.gateThreshold, 2) + "V", CENTER_X, VALUE_Y + 15, 1);
}


//======================================================
// drawTRIACComponent()
//
// Символ TRIAC: круг и выводы голубые,
// треугольники жёлтые, вертикальные полосы белые.
// Nano передаёт только пару Gate-Main1 и оставшийся Main2.
//======================================================
void drawTRIACComponent(ComponentResult c)
{
  const int cx = 80;
  const int cy = 63;
  const int r  = 24;

  const uint16_t lineColor = TFT_CYAN;
  const uint16_t triTopColor = PIN1_COLOR;
  const uint16_t triBottomColor = PIN2_COLOR;
  const uint16_t barColor  = TFT_WHITE;

  // Круг толщиной 2 пикселя.
  tft.drawCircle(cx, cy, r, lineColor);
  tft.drawCircle(cx, cy, r - 1, lineColor);

  // Силовые выводы 2 и 1.
  tft.drawFastHLine(cx - 48, cy, 96, lineColor);
  tft.drawFastHLine(cx - 48, cy + 1, 96, lineColor);

  // Белые вертикальные полосы внутри символа.
  tft.drawFastVLine(cx - 7, cy - 17, 35, barColor);
  tft.drawFastVLine(cx - 6, cy - 17, 35, barColor);
  tft.drawFastVLine(cx + 7, cy - 17, 35, barColor);
  tft.drawFastVLine(cx + 8, cy - 17, 35, barColor);

  // Верхний жёлтый треугольник направлен вправо.
  tft.fillTriangle(cx - 5, cy - 16,
                   cx - 5, cy,
                   cx + 7, cy - 8,
                   triTopColor);

  // Нижний жёлтый треугольник направлен влево.
  tft.fillTriangle(cx + 6, cy,
                   cx + 6, cy + 16,
                   cx - 7, cy + 8,
                   triBottomColor);

  // Вывод затвора 3 — вниз вправо.
  tft.drawLine(cx + 7, cy + 10, cx + 24, cy + 24, lineColor);
  tft.drawLine(cx + 8, cy + 10, cx + 25, cy + 24, lineColor);
  tft.drawFastHLine(cx + 24, cy + 24, 25, lineColor);
  tft.drawFastHLine(cx + 24, cy + 25, 25, lineColor);

  tft.setTextColor(TFT_CYAN, COLOR_BG);
  if (c.triacPairKnown)
  {
    String pairText = "G-M1: ";
    pairText += pinName(c.triacPairPin1);
    pairText += ",";
    pairText += pinName(c.triacPairPin2);
    tft.drawCentreString(pairText, CENTER_X, 96, 1);

    String mainText = "M2: ";
    mainText += pinName(c.triacMainPin);
    tft.drawCentreString(mainText, CENTER_X, 105, 1);
  }
  else
  {
    tft.drawCentreString("G-M1: ?", CENTER_X, 99, 1);
  }
}

//======================================================
// drawComponent()
//
// Универсальная функция рисования компонента.
//
// Теперь Tester.ino не должен сам решать,
// как рисовать резистор, диод или конденсатор.
//
// Он просто передаёт сюда ComponentResult.
//======================================================

void drawComponent(ComponentResult c)
{
  // Полностью очищаем рабочую область внутри синей рамки
  // Это убирает остатки старого компонента.
  tft.fillRect(2, 20, 156, 90, COLOR_BG);

  //====================================================
  // Если ничего не найдено
  //====================================================

  if (c.type == COMP_NONE)
  {
    drawSearchNoComponentScreen();
    return;
  }

  //====================================================
  // Резистор
  //====================================================

  if (c.type == COMP_RESISTOR)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);
    tft.drawCentreString(txtResistor(), CENTER_X, COMPONENT_Y, 2);

    drawResistorComponent(c.pinA, c.pinB, c.resistance);

    tft.setTextColor(TFT_WHITE, COLOR_BG);
    tft.drawCentreString(formatResistance(c.resistance), CENTER_X, VALUE_Y, 2);

    return;
  }

  //====================================================
  // Диод
  //====================================================

  if (c.type == COMP_DIODE)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);
    tft.drawCentreString(txtDiode(), CENTER_X, COMPONENT_Y, 2);

    drawDiodeComponent(c.pinA, c.pinB, c.voltage);

    return;
  }

  //====================================================
  // Светодиод
  //====================================================

  if (c.type == COMP_LED)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);
    tft.drawCentreString(txtLED(), CENTER_X, COMPONENT_Y, 2);

    drawDiodeComponent(c.pinA, c.pinB, c.voltage, ledTriangleColor(c.voltage));

    return;
  }

  //====================================================
  // Конденсатор
  //====================================================

  if (c.type == COMP_CAPACITOR)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);

    int capTitleY = COMPONENT_Y;
    if (languageMode == LANG_RU)
      capTitleY = COMPONENT_Y - 6;

    tft.drawCentreString(txtCapacitor(), CENTER_X, capTitleY, 2);

    drawCapacitorComponent(c.pinA, c.pinB, c.capacitance, c.esr, c.vloss);

    return;
  }

  //====================================================
  // MOSFET
  //====================================================

  if (c.type == COMP_NMOS || c.type == COMP_PMOS)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);

    if (c.type == COMP_NMOS)
      tft.drawCentreString("N-MOSFET", CENTER_X, COMPONENT_Y - 5, 2);
    else
      tft.drawCentreString("P-MOSFET", CENTER_X, COMPONENT_Y - 5, 2);

    drawMOSFETComponent(c);

    return;
  }

  //====================================================
  // NPN / PNP транзистор
  //====================================================

  if (c.type == COMP_NPN || c.type == COMP_PNP)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);

    if (c.type == COMP_NPN)
      tft.drawCentreString("NPN", CENTER_X, COMPONENT_Y - 5, 2);
    else
      tft.drawCentreString("PNP", CENTER_X, COMPONENT_Y - 5, 2);

    drawBJTComponent(c);

    return;
  }

  //====================================================
  // Симистор TRIAC
  //====================================================
  if (c.type == COMP_TRIAC)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);
    tft.drawCentreString("TRIAC", CENTER_X, COMPONENT_Y - 5, 2);
    drawTRIACComponent(c);
    return;
  }

  //====================================================
  // Катушка индуктивности
  //====================================================

  if (c.type == COMP_INDUCTOR)
  {
    tft.setTextColor(TFT_GREEN, COLOR_BG);
    tft.drawCentreString("INDUCTOR", CENTER_X, COMPONENT_Y, 2);
    drawInductorComponent(c.pinA, c.pinB, c.inductance);

    // Сопротивление обмотки и индуктивность одной строкой.
    String coilR;
    if (c.resistance < 10.0f)
      coilR = String(c.resistance, 1) + " Ohm";
    else if (c.resistance < 1000.0f)
      coilR = String(c.resistance, 0) + " Ohm";
    else
      coilR = String(c.resistance / 1000.0f, 2) + " kOhm";

    // Индуктивность показываем в mH, как в строке Nano.
    // Для значений меньше 1 mH убираем ведущий ноль:
    // 0.24 mH -> .24 mH
    String coilL = String(c.inductance, 2) + " mH";
    if (coilL.startsWith("0."))
      coilL.remove(0, 1);
    String coilLine = "R=" + coilR + "  L=" + coilL;

    tft.setTextColor(TFT_CYAN, COLOR_BG);
    tft.drawCentreString(coilLine, CENTER_X, VALUE_Y + 8, 1);
    return;
  }

  //====================================================
  // Неизвестный компонент
  //====================================================

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  
  tft.drawCentreString(txtUnknown(), CENTER_X, 60, 2);
}

//======================================================
// formatResistance()
//
// Форматирует сопротивление.
//======================================================

String formatResistance(float r)
{
  if (r < 1000.0)
  {
    return String(r, 0) + " Ohm";
  }

  if (r < 1000000.0)
  {
    return String(r / 1000.0, 2) + " kOhm";
  }

  return String(r / 1000000.0, 2) + " MOhm";
}

//======================================================
// formatCapacitance()
//
// На входе ёмкость в uF.
//======================================================

String formatCapacitance(float capUF)
{
  if (capUF <= 0.0)
    return "0.00 uF";

  if (capUF < 0.001)
    return String(capUF * 1000000.0, 0) + " pF";

  if (capUF < 1.0)
    return String(capUF * 1000.0, 1) + " nF";

  if (capUF < 10.0)
    return String(capUF, 2) + " uF";

  if (capUF < 100.0)
    return String(capUF, 1) + " uF";

  return String(capUF, 0) + " uF";
}

//======================================================
// drawTesterTopBar()
//
// Верхняя строка экрана TESTER.
//======================================================

void drawTesterTopBar()
{
  tft.fillRect(0, 0, 160, 18, TFT_BLACK);

  float v = getBatteryVoltage();

  // ВАЖНО: в режиме TESTER верхняя строка рисуется своей функцией,
  // а не drawBattery(). Поэтому критическое отключение нужно вызвать здесь,
  // иначе в меню разряд отключает, а в тестере нет.
  criticalBatteryShutdown(v);

  float showVoltage = v;
  if (showVoltage > 4.20) showVoltage = 4.20;
  if (showVoltage < 3.38) showVoltage = 3.38;

  int percent = (showVoltage - 3.38) * 100.0 / (4.20 - 3.38);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  uint16_t c = TFT_GREEN;
  if (percent <= 25) c = TFT_RED;
  else if (percent <= 60) c = TFT_YELLOW;

  // Батарейка
  tft.drawRect(5, 4, 24, 10, TFT_WHITE);
  tft.fillRect(29, 7, 3, 4, TFT_WHITE);
  tft.fillRect(8, 6, 19, 6, TFT_BLACK);

  int fill = map(percent, 0, 100, 0, 19);
  if (fill > 0) tft.fillRect(8, 6, fill, 6, c);

  // Вольты
  tft.setTextColor(c, TFT_BLACK);
  tft.drawString(String(v, 2) + "V", 34, 6, 1);

  // Название
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString("MULTITESTER", 100, 6, 1);

  // Проценты
  tft.setTextColor(c, TFT_BLACK);
  tft.drawRightString(String(percent) + "%", 156, 6, 1);
}

//======================================================
// drawTesterFrame()
//
// Синяя рамка экрана TESTER.
//======================================================

void drawTesterFrame()
{
  uint16_t c = TFT_BLUE;

  tft.drawRect(0, 18, 160, 94, c);

  tft.drawLine(0, 18, 8, 28, c);
  tft.drawLine(159, 18, 151, 28, c);

  tft.drawLine(0, 111, 8, 103, c);
  tft.drawLine(159, 111, 151, 103, c);

  tft.drawFastHLine(0, 108, 160, c);
}
//======================================================
// drawBJTComponent()
//
// Рисует компактный символ NPN / PNP.
//
// c.pinA = база
// c.pinB = пока условно коллектор
// c.pinC = пока условно эмиттер
//======================================================

void drawBJTComponent(ComponentResult c)
{
  int cy = RESISTOR_Y - 4;

  uint16_t baseColor = pinColor(c.pinA);
  uint16_t cColor    = pinColor(c.pinB);
  uint16_t eColor    = pinColor(c.pinC);

  // Номера щупов
  tft.setTextColor(baseColor, COLOR_BG);
  tft.drawString(pinName(c.pinA), 15, cy - 7, 2);

  tft.setTextColor(cColor, COLOR_BG);
  tft.drawString(pinName(c.pinB), 125, cy - 30, 2);

  tft.setTextColor(eColor, COLOR_BG);
  tft.drawString(pinName(c.pinC), 125, cy + 16, 2);

  // База
  tft.drawFastHLine(42, cy, 26, TFT_CYAN);
  tft.drawFastVLine(67, cy - 13, 26, TFT_WHITE);

  // Коллектор и эмиттер
  tft.drawLine(67, cy - 1, 96, cy - 18, TFT_CYAN);
  tft.drawLine(67, cy + 1, 96, cy + 18, TFT_CYAN);

  // Круг
  tft.drawCircle(77, cy, 20, TFT_CYAN);

  // Стрелка
  if (c.type == COMP_NPN)
  {
    tft.fillTriangle(77, cy + 12, 90, cy + 14, 81, cy + 3, TFT_YELLOW);
    
  }
  else
  {
    tft.fillTriangle(77, cy + 12, 67, cy + 1, 81, cy + 3, TFT_YELLOW);
  }

  // B C E
  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.drawString("B", 43, cy - 12, 1);
  tft.drawString("C", 100, cy - 25, 1);
  tft.drawString("E", 100, cy + 24, 1);
  
  //====================================================
  // Параметры транзистора
  //
  // hFE = 120
  // Uf  = 0.65V
  //====================================================

  tft.setTextColor(TFT_WHITE, COLOR_BG);

  tft.drawCentreString(
  "hFE=" + String(c.hFE, 0) + " Uf=" + String(c.voltage, 2) + "V",
  CENTER_X,
  VALUE_Y + 15,
  1
);
  
}

//======================================================
// drawInductorComponent()
// Рисует катушку, значение inductance передаётся в mH.
//======================================================
void drawInductorComponent(uint8_t pinA, uint8_t pinB, float inductance)
{
  int y = RESISTOR_Y;

  tft.setTextColor(pinColor(pinA), COLOR_BG);
  tft.drawString(pinName(pinA), 22, y - 12, 4);
  tft.setTextColor(pinColor(pinB), COLOR_BG);
  tft.drawString(pinName(pinB), 132, y - 12, 4);

  tft.drawFastHLine(39, y, 19, TFT_CYAN);
  tft.drawFastHLine(102, y, 19, TFT_CYAN);

  // Четыре витка катушки.
  for (int i = 0; i < 4; i++)
    tft.drawCircle(63 + i * 11, y, 8, TFT_YELLOW);

  // Закрываем нижние половины окружностей, оставляя классический символ катушки.
  tft.fillRect(54, y + 1, 51, 9, COLOR_BG);
  tft.drawFastHLine(55, y, 48, TFT_YELLOW);

  String value;
  if (inductance < 0.001f)
    value = String(inductance * 1000000.0f, 0) + " nH";
  else if (inductance < 1.0f)
    value = String(inductance * 1000.0f, 1) + " uH";
  else if (inductance < 1000.0f)
    value = String(inductance, 2) + " mH";
  else
    value = String(inductance / 1000.0f, 2) + " H";

  // Значение катушки выводится одной строкой вместе с R в drawDetectedComponent().
}
