#include "Config.h"
#include <hardware/adc.h>

//======================================================
// Реальное измерение аккумулятора / питания Pico
//
// Источник найден через ADC Debug:
//   ADC3 / GP29 меняется вместе с VSYS
//   OLD BATT показывает правильное напряжение
//
// Поэтому читаем точно как в старом рабочем скетче:
//   adc_gpio_init(29);
//   adc_select_input(3);
//   voltage = raw * 0.002417
//======================================================

float getBatteryVoltage()
{
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);      // ADC3 / GP29

    uint32_t raw = 0;

    // После выбора ADC3 первые чтения могут быть мусорными,
    // поэтому несколько замеров выбрасываем.
    for (int i = 0; i < 4; i++)
    {
        adc_read();
        delayMicroseconds(20);
    }

    for (int i = 0; i < 20; i++)
    {
        raw += adc_read();
        delayMicroseconds(20);
    }

    float avg = raw / 20.0f;
    return avg * 0.002417f;   // коэффициент из старого скетча
}

//======================================================
// Критический разряд аккумулятора
// 3.39 В и ниже: предупреждение 3 секунды, три пика,
// затем выключаем подсветку и останавливаем программу.
// Пробуждение только перезагрузкой / передёргиванием питания.
//======================================================

const float BATTERY_CRITICAL_VOLTAGE = 3.39f;

// Защита от ложного отключения при старте.
// После включения питание/ADC несколько секунд стабилизируются,
// поэтому критический разряд начинаем проверять только позже.
const unsigned long BATTERY_STARTUP_GUARD_MS = 6000UL;

// Отключаемся только после нескольких низких замеров подряд,
// а не по одному случайному скачку ADC.
const uint8_t BATTERY_LOW_CONFIRM_COUNT = 3;

const uint8_t LB_B[7]  = { B11110, B10000, B10000, B11110, B10001, B10001, B11110 }; // Б
const uint8_t LB_A[7]  = { B01110, B10001, B10001, B11111, B10001, B10001, B10001 }; // А
const uint8_t LB_T[7]  = { B11111, B00100, B00100, B00100, B00100, B00100, B00100 }; // Т
const uint8_t LB_E[7]  = { B11111, B10000, B10000, B11110, B10000, B10000, B11111 }; // Е
const uint8_t LB_R[7]  = { B11110, B10001, B10001, B11110, B10000, B10000, B10000 }; // Р
const uint8_t LB_YA[7] = { B01111, B10001, B10001, B01111, B00101, B01001, B10001 }; // Я
const uint8_t LB_Z[7]  = { B11110, B00001, B00001, B00110, B00001, B00001, B11110 }; // З
const uint8_t LB_ZH[7] = { B10101, B10101, B10101, B01110, B10101, B10101, B10101 }; // Ж
const uint8_t LB_N[7]  = { B10001, B10001, B10001, B11111, B10001, B10001, B10001 }; // Н

void drawLowBatteryGlyph(const uint8_t glyph[7], int x, int y, uint8_t scale, uint16_t color)
{
    for (uint8_t row = 0; row < 7; row++)
    {
        for (uint8_t col = 0; col < 5; col++)
        {
            if (glyph[row] & (1 << (4 - col)))
                tft.fillRect(x + col * scale, y + row * scale, scale, scale, color);
        }
    }
}

void drawLowBatteryWord1(int x, int y, uint8_t scale, uint16_t color)
{
    const uint8_t* letters[] = { LB_B, LB_A, LB_T, LB_A, LB_R, LB_E, LB_YA };
    for (uint8_t i = 0; i < 7; i++)
        drawLowBatteryGlyph(letters[i], x + i * 12, y, scale, color);
}

void drawLowBatteryWord2(int x, int y, uint8_t scale, uint16_t color)
{
    const uint8_t* letters[] = { LB_R, LB_A, LB_Z, LB_R, LB_YA, LB_ZH, LB_E, LB_N, LB_A };
    for (uint8_t i = 0; i < 9; i++)
        drawLowBatteryGlyph(letters[i], x + i * 12, y, scale, color);
}


//======================================================
// Максимально глубокий программный сон после критического
// разряда. Пробуждение только RESET / передёргивание питания.
//
// Важно: настоящий "0 мА" на Pico возможен только аппаратно
// через выключение питания MOSFET/ключом. Здесь мы программно:
// - гасим подсветку TFT;
// - отправляем дисплей в Sleep In;
// - переводим внешние пины в безопасное Hi-Z состояние;
// - останавливаем ядро в WFI-цикле без пробуждения по кнопке.
//======================================================

void lowBatteryDeepSleepForever()
{
    digitalWrite(BUZZER_PIN, LOW);
    pinMode(BUZZER_PIN, OUTPUT);

    // Выключить подсветку экрана
    backlightOff();

    // Попросить контроллер TFT выключить изображение и уйти в сон
    // 0x28 = Display OFF, 0x10 = Sleep IN для ST77xx/ILI-подобных TFT
    tft.writecommand(0x28);
    delay(20);
    tft.writecommand(0x10);
    delay(120);

    // Все измерительные цепи и кнопки в Hi-Z, чтобы не тянуть ток
    pinMode(IR_PIN, INPUT);
    pinMode(BTN_UP, INPUT);
    pinMode(BTN_DOWN, INPUT);
    pinMode(BTN_OK, INPUT);

    noInterrupts();

    while (true)
    {
        // Остановить ядро до любого аппаратного события.
        // Так как прерывания выключены и обработчиков пробуждения нет,
        // штатный выход только через RESET или снятие/подачу питания.
        asm volatile("wfi");
    }
}

void criticalBatteryShutdown(float voltage)
{
    static bool alreadyStarted = false;
    static uint8_t lowCount = 0;

    if (alreadyStarted) return;

    // При старте не выключаем прибор, даже если первый ADC-замер ложный.
    if (millis() < BATTERY_STARTUP_GUARD_MS)
    {
        lowCount = 0;
        return;
    }

    if (voltage > BATTERY_CRITICAL_VOLTAGE)
    {
        lowCount = 0;
        return;
    }

    lowCount++;
    if (lowCount < BATTERY_LOW_CONFIRM_COUNT)
    {
        return;
    }

    alreadyStarted = true;

    backlightOn();
    applyDisplayBrightness();

    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, TFT_RED);

    drawLowBatteryWord1(38, 34, 2, TFT_RED);  // БАТАРЕЯ
    drawLowBatteryWord2(26, 62, 2, TFT_RED);  // РАЗРЯЖЕНА

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString(String(voltage, 2) + "V", CENTER_X, 96, 2);

    soundBatteryEmpty();
    delay(3000);

    // Перед окончательным сном отключаем MT3608 и Nano.
    nanoPowerOff();

    tft.fillScreen(TFT_BLACK);
    lowBatteryDeepSleepForever();
}


//======================================================
// Проценты батареи для верхней строки
//======================================================

int getBatteryPercent(float voltage)
{
    float showVoltage = voltage;

    if (showVoltage > 4.20f) showVoltage = 4.20f;
    if (showVoltage < 3.38f) showVoltage = 3.38f;

    int percent = (int)(((showVoltage - 3.38f) * 100.0f) / (4.20f - 3.38f));

    if (percent > 100) percent = 100;
    if (percent < 0) percent = 0;

    return percent;
}

//======================================================
// Значок батареи
//======================================================

void drawBattery(float voltage)
{
    criticalBatteryShutdown(voltage);

    //--------------------------------------------------
    // Тройной пик при полном разряде аккумулятора.
    // Пока нижний реальный предел у Жени получается около 3.38 В,
    // поэтому тревогу ставим мягко: 3.40 В.
    // Срабатывает один раз, потом сбрасывается выше 3.55 В.
    //--------------------------------------------------

    // ВАЖНО: пока только отображаем батарею.
    // Тревогу разряда временно отключили, чтобы прибор не зависал
    // и не пищал при тестах лабораторником.

    // Для шкалы берём рабочий диапазон как в твоей плате:
    // 3.38 В = почти пусто, 4.20 В = полный заряд.
    float showVoltage = voltage;
    if (showVoltage > 4.20) showVoltage = 4.20;
    if (showVoltage < 3.38) showVoltage = 3.38;

    int percent = getBatteryPercent(voltage);

    const int x = 4;
    const int y = 3;
    const int w = 24;
    const int h = 10;

    // Очистить зону батарейки и напряжения
    tft.fillRect(2, 1, 88, 14, TFT_DARKGREY);

    // Контур
    tft.drawRect(x, y, w, h, TFT_WHITE);
    tft.fillRect(x + w, y + 3, 2, 4, TFT_WHITE);

    int fill = map(percent, 0, 100, 0, w - 4);

    uint16_t color;
    if (percent > 60)
        color = TFT_GREEN;
    else if (percent > 25)
        color = TFT_YELLOW;
    else
        color = TFT_RED;

    tft.fillRect(x + 2, y + 2, w - 4, h - 4, TFT_BLACK);

    if (fill > 0)
        tft.fillRect(x + 2, y + 2, fill, h - 4, color);

    // Напряжение рядом с батарейкой
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString(String(voltage, 2) + "V", 34, 2, 2);
}


//======================================================
// Батарея на заставке без серой полосы
// Только значок батареи + вольты, без процентов.
// Фон очищаем цветом верхней половины украинского флага.
//======================================================

void drawSplashBattery()
{
    const uint16_t splashBg = bootBackgroundColor();
    float voltage = getBatteryVoltage();
    criticalBatteryShutdown(voltage);
    int percent = getBatteryPercent(voltage);

    const int x = 4;
    const int y = 3;
    const int w = 24;
    const int h = 10;

    // Очищаем только маленькую область батарейки и вольт,
    // а не всю верхнюю полосу, чтобы не портить флаг.
    tft.fillRect(2, 1, 88, 14, splashBg);

    tft.drawRect(x, y, w, h, TFT_WHITE);
    tft.fillRect(x + w, y + 3, 2, 4, TFT_WHITE);

    int fill = map(percent, 0, 100, 0, w - 4);

    uint16_t color;
    if (percent > 60)
        color = TFT_GREEN;
    else if (percent > 25)
        color = TFT_YELLOW;
    else
        color = TFT_RED;

    tft.fillRect(x + 2, y + 2, w - 4, h - 4, TFT_BLACK);

    if (fill > 0)
        tft.fillRect(x + 2, y + 2, fill, h - 4, color);

    tft.setTextColor(TFT_WHITE, splashBg);
    tft.drawString(String(voltage, 2) + "V", 34, 2, 2);
}
