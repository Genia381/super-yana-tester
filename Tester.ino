/****************************************************************
                SUPER YANA TESTER
                File: Tester.ino

  Рабочий режим тестера:
  - измерения выполняет Arduino Nano;
  - Pico отправляет @TEST и принимает результат по UART;
  - короткое OK запускает новое измерение;
  - длинное OK возвращает в меню.
****************************************************************/

#include "Config.h"

#define TESTER_OK_LONG_TIME 2200

void drawTesterScreen()
{
  clearScreen();
  drawTesterTopBar();
  drawTesterFrame();

  tft.fillRect(2, 20, 156, 90, COLOR_BG);
  tft.fillRect(2, 112, 156, 16, TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  drawLocalizedTesterHint(CENTER_X, 114, TFT_GREEN);
}

void testerLoop()
{
  drawTesterScreen();
  nanoTesterScreenInit();
  delay(100);
  nanoRequestMeasurement();

  unsigned long batteryTimer = 0;

  while (true)
  {
    autoOffTick();

    if (digitalRead(BTN_OK) == HIGH)
    {
      delay(35);

      if (digitalRead(BTN_OK) == HIGH)
      {
        userActivity();
        unsigned long startPress = millis();

        while (digitalRead(BTN_OK) == HIGH)
        {
          nanoUartPoll();

          if (millis() - startPress >= TESTER_OK_LONG_TIME)
          {
            soundLong();
            drawMenu(true);

            while (digitalRead(BTN_OK) == HIGH)
              delay(5);

            delay(100);
            return;
          }

          delay(5);
        }

        delay(80);
        soundClick();
        nanoRequestMeasurement();
      }
    }

    nanoUartPoll();

    if (millis() - batteryTimer >= 1000)
    {
      batteryTimer = millis();
      drawTesterTopBar();
    }

    delay(2);
  }
}
