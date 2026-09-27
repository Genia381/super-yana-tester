#include "Config.h"

//======================================================
// AUTO OFF
//
// Это не глубокий sleep RP2040, а безопасный сон прибора:
// - гасим подсветку TFT;
// - ждём короткое OK;
// - после OK включаем подсветку обратно.
//======================================================

unsigned long lastUserActivity = 0;
bool autoSleepActive = false;

void userActivity()
{
  lastUserActivity = millis();
}

unsigned long autoOffIntervalMs()
{
  switch (autoOffMode)
  {
    case 0: return 60000UL;    // 1 min
    case 1: return 120000UL;   // 2 min
    case 2: return 300000UL;   // 5 min
    default: return 0UL;       // Off
  }
}

void enterAutoSleep()
{
  autoSleepActive = true;

  // Измерительные линии Pico больше не используются:
  // все измерения компонентов выполняет Nano по UART.

  // При обычном авто-сне Nano и MT3608 остаются включёнными.
  // Гасим только подсветку Pico, чтобы пробуждение было быстрым.
  backlightOff();

  // Ждём короткое OK для пробуждения.
  // Экран не очищаем, чтобы после включения подсветки картинка осталась.
  while (digitalRead(BTN_OK) == HIGH)
  {
    delay(10);
  }

  while (digitalRead(BTN_OK) == LOW)
  {
    delay(20);
  }

  delay(40);

  while (digitalRead(BTN_OK) == HIGH)
  {
    delay(10);
  }

  // Nano всё это время работала. После пробуждения восстанавливаем
  // UART Pico прямо здесь. Экран при этом не перерисовываем и
  // команду @TEST автоматически не отправляем.
  nanoResetAfterWake();

  backlightOn();
  applyDisplayBrightness();
  delay(80);
  soundClick();

  // Нажатие, которым прибор был разбужен, полностью поглощается здесь.
  // Оно только включает подсветку и восстанавливает UART.
  // Команда @TEST будет отправлена только следующим отдельным нажатием OK.

  userActivity();
  autoSleepActive = false;
}

bool autoOffTick()
{
  // Любое нажатие OK сразу запускает выбранное время до сна заново.
  // Не ждём отпускания кнопки и не зависим от того, какой экран открыт.
  if (digitalRead(BTN_OK) == HIGH)
  {
    userActivity();
  }

  unsigned long interval = autoOffIntervalMs();
  if (interval == 0) return false;
  if (autoSleepActive) return false;

  if (lastUserActivity == 0)
    userActivity();

  if (millis() - lastUserActivity >= interval)
  {
    enterAutoSleep();

    // Кнопка пробуждения только включает подсветку.
    // Возвращаем false, чтобы текущий экран после сна не перерисовывался.
    return false;
  }

  return false;
}
