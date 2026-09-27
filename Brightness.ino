/****************************************************************
                YANA MULTITESTER
                File: Brightness.ino

  Ступенчатая аппаратная яркость без PWM.

  Теперь подсветка BL питается через отдельные резисторы
  от пинов GP10, GP11, GP12, GP13.

  displayBrightness:
    0 = 25%  -> GP11  (по фактической проверке самый тёмный)
    1 = 50%  -> GP12
    2 = 75%  -> GP13
    3 = 100% -> GP10  (по фактической проверке самый яркий)

  Переназначено по реальному результату на макетке:
    пункт 25% раньше был самый яркий, поэтому его пин стал 100%
    пункт 50% раньше был самый тёмный, поэтому его пин стал 25%

  Остальные пины яркости всегда переводятся в INPUT, чтобы они
  не спорили друг с другом.
****************************************************************/

#include "Config.h"

#define BL_25_PIN   11
#define BL_50_PIN   12
#define BL_75_PIN   13
#define BL_100_PIN  10

uint8_t displayBrightness = 3; // 0=25%, 1=50%, 2=75%, 3=100%

uint8_t clampDisplayBrightness(uint8_t value)
{
  if (value > 3) return 3;
  return value;
}

uint8_t displayBrightnessPwmValue(uint8_t value)
{
  // Оставлено только для совместимости со старыми объявлениями.
  // В этой версии PWM не используется.
  switch (clampDisplayBrightness(value))
  {
    case 0: return 64;
    case 1: return 128;
    case 2: return 192;
    default: return 255;
  }
}

void allBacklightStepPinsHiZ()
{
  digitalWrite(BL_25_PIN, LOW);
  digitalWrite(BL_50_PIN, LOW);
  digitalWrite(BL_75_PIN, LOW);
  digitalWrite(BL_100_PIN, LOW);

  pinMode(BL_25_PIN, INPUT);
  pinMode(BL_50_PIN, INPUT);
  pinMode(BL_75_PIN, INPUT);
  pinMode(BL_100_PIN, INPUT);

  // GP15 теперь управляет питанием Nano через MOSFET.
  // Здесь его не трогаем.
}

void setBacklightStep(uint8_t value)
{
  uint8_t level = clampDisplayBrightness(value);

  allBacklightStepPinsHiZ();

  uint8_t pin;
  if (level == 0) pin = BL_25_PIN;
  else if (level == 1) pin = BL_50_PIN;
  else if (level == 2) pin = BL_75_PIN;
  else pin = BL_100_PIN;

  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void backlightFullOn()
{
  // Полная яркость через фактически самый яркий пин GP10.
  setBacklightStep(3);
}

void backlightOn()
{
  // Включить сохранённую яркость после сна/пробуждения.
  applyDisplayBrightness();
}

void backlightOff()
{
  allBacklightStepPinsHiZ();
}

void previewDisplayBrightness(uint8_t value)
{
  // Теперь предпросмотр безопасный: без PWM, просто переключаем пины.
  setBacklightStep(value);
}

void applyDisplayBrightness()
{
  // Применение сохранённой яркости после загрузки/пробуждения.
  setBacklightStep(displayBrightness);
}

const char* displayBrightnessName()
{
  switch (clampDisplayBrightness(displayBrightness))
  {
    case 0: return "25%";
    case 1: return "50%";
    case 2: return "75%";
    default: return "100%";
  }
}

void saveDisplayBrightness(uint8_t value)
{
  displayBrightness = clampDisplayBrightness(value);

  // Сохраняем в общую память настроек.
  // Сама функция saveUserSettings() лежит в Settings.ino.
  saveUserSettings();

  // В этой версии можно сразу применять яркость — PWM нет,
  // поэтому экран не должен уходить в чёрный.
  applyDisplayBrightness();
}
