#include "Config.h"

//======================================================
// Настройки кнопок
// TTP223: нет касания = LOW, касание = HIGH
//======================================================

#define BUTTON_DEBOUNCE 40
#define BUTTON_AFTER_PRESS_DELAY 120

// После долгого OK кнопку надо игнорировать до отпускания,
// чтобы новый экран не принял удержание как короткое нажатие.
static bool okIgnoreUntilRelease = false;

//======================================================
// Ждать отпускания кнопки
//======================================================

void waitButtonRelease(uint8_t pin)
{
  while (digitalRead(pin) == HIGH)
  {
    delay(5);
  }

  delay(BUTTON_AFTER_PRESS_DELAY);
}

//======================================================
// Универсальное короткое нажатие
//======================================================

bool buttonClick(uint8_t pin)
{
  //----------------------------------------------------
  // Если до этого было долгое OK, не превращаем
  // отпускание в короткое нажатие.
  //----------------------------------------------------
  if (pin == BTN_OK && okIgnoreUntilRelease)
  {
    if (digitalRead(BTN_OK) == LOW)
    {
      okIgnoreUntilRelease = false;
      delay(BUTTON_AFTER_PRESS_DELAY);
    }

    return false;
  }

  if (digitalRead(pin) == HIGH)
  {
    delay(BUTTON_DEBOUNCE);

    if (digitalRead(pin) == HIGH)
    {
      // Любое подтверждённое нажатие сразу начинает отсчёт сна заново.
      // Для OK это действует во всех меню и режимах, где используется buttonOK().
      userActivity();
      waitButtonRelease(pin);
      userActivity();
      soundClick();
      return true;
    }
  }

  return false;
}

//======================================================
// Вверх GP6
//======================================================

bool buttonUp()
{
  return buttonClick(BTN_UP);
}

//======================================================
// Вниз GP7
//======================================================

bool buttonDown()
{
  return buttonClick(BTN_DOWN);
}

//======================================================
// OK GP22 короткое нажатие
//======================================================

bool buttonOK()
{
  return buttonClick(BTN_OK);
}

//======================================================
// OK долгое нажатие
//
// ВАЖНО:
// Возвращает true СРАЗУ после выдержки holdTime.
// Не ждёт отпускания кнопки. Поэтому экран реагирует сразу.
// После этого короткое OK игнорируется до отпускания.
//======================================================

bool buttonOKLong(uint16_t holdTime)
{
  if (okIgnoreUntilRelease)
  {
    if (digitalRead(BTN_OK) == LOW)
    {
      okIgnoreUntilRelease = false;
      delay(BUTTON_AFTER_PRESS_DELAY);
    }

    return false;
  }

  if (digitalRead(BTN_OK) == HIGH)
  {
    delay(BUTTON_DEBOUNCE);

    if (digitalRead(BTN_OK) == HIGH)
    {
      // Сбрасываем таймер сна сразу при подтверждении нажатия,
      // не дожидаясь определения короткого или долгого OK.
      userActivity();
      unsigned long startTime = millis();

      while (digitalRead(BTN_OK) == HIGH)
      {
        if (millis() - startTime >= holdTime)
        {
          okIgnoreUntilRelease = true;
          userActivity();
          soundLong();
          return true;
        }

        delay(5);
      }
    }
  }

  return false;
}
