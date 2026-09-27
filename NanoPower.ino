#include "Config.h"

//======================================================
// Питание измерительной головы Nano через MT3608.
// GP15 управляет N-MOSFET 50N024 в разрыве минуса MT3608.
// HIGH = Nano включена, LOW = Nano выключена.
//======================================================

#define NANO_POWER_PIN 15
#define NANO_BOOT_DELAY_MS 1800UL

static bool nanoPowerIsOn = false;

void nanoPowerInit()
{
  pinMode(NANO_POWER_PIN, OUTPUT);
  digitalWrite(NANO_POWER_PIN, HIGH);
  nanoPowerIsOn = true;

  // Дать MT3608 и Nano время стабилизироваться перед запуском UART.
  delay(NANO_BOOT_DELAY_MS);
}

void nanoPowerOff()
{
  if (!nanoPowerIsOn)
    return;

  // Убираем возможную подпитку Nano через линии UART.
  Serial1.end();
  pinMode(0, INPUT);
  pinMode(1, INPUT);

  delay(10);
  digitalWrite(NANO_POWER_PIN, LOW);
  nanoPowerIsOn = false;
}

void nanoPowerOn()
{
  if (nanoPowerIsOn)
    return;

  digitalWrite(NANO_POWER_PIN, HIGH);
  nanoPowerIsOn = true;

  // Ожидаем загрузку Nano, затем снова запускаем UART GP0/GP1.
  delay(NANO_BOOT_DELAY_MS);
  nanoUartInit();
}

bool nanoPowerState()
{
  return nanoPowerIsOn;
}
