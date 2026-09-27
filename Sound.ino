/****************************************************************
                SUPER YANA TESTER v2
                File: Sound.ino

  Звук для пассивного PC-бузера.

  Громкость регулируется программно:
  - меняется скважность прямоугольного сигнала;
  - 0 = OFF;
  - 1...4 = 25...100%.
****************************************************************/

#include "Config.h"

bool soundEnabled = true;
uint8_t soundVolume = 4;   // 0=Off, 1=25%, 2=50%, 3=75%, 4=100%

//======================================================
// soundInit()
//======================================================

void soundInit()
{
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

//======================================================
// soundVolumeName()
//======================================================

const char* soundVolumeName()
{
  switch (soundVolume)
  {
    case 0: return "Off";
    case 1: return "25%";
    case 2: return "50%";
    case 3: return "75%";
    default: return "100%";
  }
}

//======================================================
// soundTone()
//
// Ручная генерация тона для пассивного бузера.
// freq = частота в Гц
// ms   = длительность в миллисекундах
//======================================================

void soundTone(uint16_t freq, uint16_t ms)
{
  if (!soundEnabled || soundVolume == 0)
    return;

  if (freq == 0 || ms == 0)
  {
    digitalWrite(BUZZER_PIN, LOW);
    delay(ms);
    return;
  }

  //----------------------------------------------------
  // Настоящая регулировка громкости пассивного буззера.
  //
  // Частота и длительность звука остаются одинаковыми,
  // а громкость меняется скважностью импульсов:
  // 25%  -> 5% периода сигнал HIGH
  // 50%  -> 15% периода сигнал HIGH
  // 75%  -> 30% периода сигнал HIGH
  // 100% -> 50% периода сигнал HIGH
  //
  // При 50% скважности буззер звучит максимально громко.
  //----------------------------------------------------

  uint8_t dutyPercent;

  switch (soundVolume)
  {
    case 1: dutyPercent = 5;  break;
    case 2: dutyPercent = 15; break;
    case 3: dutyPercent = 30; break;
    default: dutyPercent = 50; break;
  }

  unsigned long periodUs = 1000000UL / freq;
  unsigned long highUs = (periodUs * dutyPercent) / 100UL;

  // Не допускаем слишком короткого импульса.
  if (highUs < 8)
    highUs = 8;

  if (highUs >= periodUs)
    highUs = periodUs / 2;

  unsigned long lowUs = periodUs - highUs;
  unsigned long cycles = ((unsigned long)freq * ms) / 1000UL;

  if (cycles < 1)
    cycles = 1;

  for (unsigned long i = 0; i < cycles; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(highUs);

    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(lowUs);
  }

  digitalWrite(BUZZER_PIN, LOW);
}

//======================================================
// Короткий звук кнопки
//======================================================

void soundClick()
{
  soundTone(2500, 25);
}

//======================================================
// Длинное OK / выход
//======================================================

void soundLong()
{
  soundTone(1500, 55);
  delay(35);
  soundTone(2200, 65);
}

//======================================================
// Вход в режим
//======================================================

void soundMode()
{
  soundTone(1800, 45);
  delay(25);
  soundTone(2600, 55);
}

//======================================================
// Компонент найден
//======================================================

void soundFound()
{
  soundTone(2300, 35);
}

//======================================================
// Ошибка / неизвестно
//======================================================

void soundError()
{
  soundTone(500, 120);
}

//======================================================
// Настройка сохранена
//======================================================

void soundSaved()
{
  soundTone(1800, 40);
  delay(25);
  soundTone(2400, 40);
  delay(25);
  soundTone(3000, 60);
}

//======================================================
// Мелодия при загрузке
//======================================================

void soundStartupMelody()
{
  if (!soundEnabled || soundVolume == 0)
    return;

  soundTone(3520, 80);
  delay(60);
  soundTone(3136, 80);
  delay(60);
  soundTone(2637, 80);
  delay(60);
  soundTone(2093, 80);
  delay(60);
  soundTone(2349, 80);
  delay(60);
  soundTone(3951, 80);
  delay(60);
  soundTone(2794, 80);
  delay(60);
  soundTone(2093, 80);
}

//======================================================
// Тройной тревожный пик при полном разряде аккумулятора
//======================================================

void soundBatteryEmpty()
{
  if (!soundEnabled || soundVolume == 0)
    return;

  for (int i = 0; i < 3; i++)
  {
    soundTone(1500, 90);
    delay(80);
  }
}
