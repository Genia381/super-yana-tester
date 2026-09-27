void drawSearchNoComponentScreen();
/*********************************************************************
 *
 *               SUPER YANA TESTER v2 alpha7 cyrillic v2 fix55 Nano UART text
 *
 *          Raspberry Pi Pico + ST7735 160x128
 *
 *********************************************************************/

#include "Config.h"

//======================================================
// Глобальный объект дисплея
//======================================================

TFT_eSPI tft = TFT_eSPI();


//======================================================
// waitStartupScreenOK()
//
// Заставка остаётся на экране бесконечно.
// Короткое OK  - старт сохранённого режима.
// Длинное OK   - сразу вход в меню, не на отпускание.
//======================================================

void waitStartupScreenOK()
{
  const unsigned long LONG_OK_TIME = 2200;
  unsigned long batteryTimer = 0;

  while (true)
  {
    bootMarqueeTick();

    if (autoOffTick())
    {
      drawSplashBattery();
    }

    // На заставке тоже обновляем батарею в реальном времени
    if (millis() - batteryTimer >= 1000)
    {
      batteryTimer = millis();
      drawSplashBattery();
    }

    if (digitalRead(BTN_OK) == HIGH)
    {
      delay(35); // антидребезг

      if (digitalRead(BTN_OK) == HIGH)
      {
        // OK на заставке тоже сразу перезапускает таймер сна.
        userActivity();
        unsigned long startPress = millis();

        while (digitalRead(BTN_OK) == HIGH)
        {
          bootMarqueeTick();

          if (millis() - startPress >= LONG_OK_TIME)
          {
            // Меню появляется сразу после выдержки, а не после отпускания.
            userActivity();
            soundLong();
            menuInit();

            // Ждём отпускание только чтобы меню не сработало повторно.
            while (digitalRead(BTN_OK) == HIGH)
            {
              delay(5);
            }
            delay(120);

            return;
          }

          delay(5);
        }

        // Короткое OK - запуск сохранённого режима.
        delay(120);
        userActivity();
        soundMode();
        startSelectedStartupMode();

        // Если из режима вышли длинным OK, показываем меню.
        menuInit();
        return;
      }
    }

    delay(10);
  }
}

//======================================================

void setup()
{
  backlightFullOn();

  pinMode(BTN_UP, INPUT_PULLDOWN);
  pinMode(BTN_DOWN, INPUT_PULLDOWN);
  pinMode(BTN_OK, INPUT_PULLDOWN);

  soundInit();

  // Включаем MT3608 и Nano до запуска UART.
  nanoPowerInit();
  nanoUartInit();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

      // Разрешение ADC 12 бит: 0...4095
   analogReadResolution(12);

  loadUserSettings();
  userActivity();

  bootAnimation();

  // Показываем батарею прямо на заставке без серой полосы
  drawSplashBattery();

  // Бегущая строка внизу заставки
  bootMarqueeInit();

  // После заставки больше ничего не рисуем поверх неё.
  // Короткое OK запускает сохранённый режим.
  // Длинное OK сразу открывает меню.
  waitStartupScreenOK();

  // После выхода из режима остаёмся в меню.
  while (true)
  {
    menuLoop();
  }
}

//======================================================

void loop()
{
  // Основная работа идёт в setup() внутри меню.
}

//======================================================
// Второй core пока не используется.
// Яркость подсветки регулируется ступенями GP10-GP13 через резисторы.
//======================================================

void setup1()
{
}

void loop1()
{
  delay(1000);
}

