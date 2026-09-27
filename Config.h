/****************************************************************
                YANA MULTITESTER
                File: Config.h

  Главный конфигурационный файл проекта.

  Здесь находятся:
  - пины;
  - цвета;
  - координаты интерфейса;
  - структуры данных;
  - прототипы функций.

****************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

//======================================================
// ГЛОБАЛЬНЫЙ ОБЪЕКТ ДИСПЛЕЯ
//======================================================

extern TFT_eSPI tft;

//======================================================
// КНОПКИ TTP223
//======================================================

#define BTN_UP         6
#define BTN_DOWN       7
#define BTN_OK         22

//======================================================
// ЗВУК
//======================================================

#define BUZZER_PIN     14

//======================================================
// ИК-ПРИЁМНИК
//======================================================

#define IR_PIN         16

//======================================================
// НОМЕРА КОНТАКТОВ ИЗМЕРИТЕЛЬНОЙ КОЛОДКИ NANO
//======================================================

#define TP1            1
#define TP2            2
#define TP3            3

//======================================================
// РАЗМЕР ЭКРАНА
//======================================================

#define LCD_WIDTH      160
#define LCD_HEIGHT     128

#define CENTER_X       80

//======================================================
// ОСНОВНЫЕ ЦВЕТА
//======================================================

#define COLOR_BG       TFT_BLACK
#define COLOR_GRID     0x0320
#define COLOR_FRAME    TFT_CYAN
#define COLOR_TEXT     TFT_WHITE
#define COLOR_TITLE    TFT_GREEN
#define COLOR_SELECT   TFT_BLUE

//======================================================
// КООРДИНАТЫ ЭКРАНА TESTER
//======================================================

#define COMPONENT_Y           28
#define RESISTOR_Y            66
#define VALUE_Y               86

//======================================================
// СТРУКТУРЫ
//======================================================

struct IRResult
{
  bool found;
  bool repeat;
  bool nec;
  uint8_t bits;
  uint32_t code;
  uint8_t address;
  uint8_t command;
};


//======================================================
// SOUND
//======================================================

extern bool soundEnabled;
extern uint8_t soundVolume;
extern unsigned long lastUserActivity;
extern bool autoSleepActive;
void soundInit();
void soundTone(uint16_t freq, uint16_t ms);
void soundClick();
void soundLong();
void soundMode();
void soundFound();
void soundError();
void soundSaved();
void soundStartupMelody();
void soundBatteryEmpty();
const char* soundVolumeName();

//======================================================
// DISPLAY
//======================================================

void clearScreen();
void drawStatusBar();
void drawFrame();
void drawBottomBar();
void drawTitle(const char *txt);
void clearWorkArea();
void drawGrid();
void centerText(const char *txt);
void bigText(const char *txt);
void drawMainScreen();

//======================================================
// BOOT
//======================================================

extern uint8_t bootStyle;
uint16_t bootBackgroundColor();
void bootAnimation();

//======================================================
// BUTTONS
//======================================================

bool buttonUp();
bool buttonDown();
bool buttonOK();
bool buttonOKLong(uint16_t holdTime);

//======================================================
// MENU
//======================================================

void menuInit();
void menuLoop();
void drawMenu(bool fullRedraw);

//======================================================
// ПРОБНИК / ADS1115 A1
//======================================================

void probeLoop();
bool adsReadSingleEndedRaw(uint8_t channel, int16_t &raw);


//======================================================
// USER SETTINGS
//======================================================

enum StartupMode
{
  STARTUP_TESTER = 0,
  STARTUP_VOLTMETER = 1
};

enum LanguageMode
{
  LANG_EN = 0,
  LANG_RU = 1,
  LANG_UA = 2
};

extern StartupMode startupMode;
extern uint8_t autoOffMode;
extern LanguageMode languageMode;
extern uint8_t displayBrightness;

void loadUserSettings();
void saveUserSettings();
float readVoltmeterValue();
extern float voltmeterCalibration;
void saveStartupMode(StartupMode mode);
void saveSoundVolume(uint8_t volume);
void saveAutoOffMode(uint8_t mode);
void saveLanguageMode(LanguageMode mode);
uint8_t clampDisplayBrightness(uint8_t value);
uint8_t displayBrightnessPwmValue(uint8_t value);
void saveDisplayBrightness(uint8_t value);
void previewDisplayBrightness(uint8_t value);
void applyDisplayBrightness();
void backlightFullOn();
void backlightOn();
void backlightOff();
const char* displayBrightnessName();
const char* startupModeName();
const char* autoOffName();
const char* languageName();

const char* txt3(const char* en, const char* ru, const char* ua);
const char* txtMainMenuTitle();
const char* txtMenuItem(uint8_t index);
const char* txtSettingsTitle();
const char* txtSettingsItem(uint8_t index);
const char* txtStartupTitle();
const char* txtStartupItem(uint8_t index);
const char* txtSoundTitle();
const char* txtAutoOffTitle();
const char* txtLanguageTitle();
const char* txtLanguageItem(uint8_t index);
const char* txtOKSelect();
const char* txtOKSave();
const char* txtSaved();
const char* txtComingSoon();
const char* txtBack();
const char* txtOKBack();
const char* txtMainMenuHint();
const char* txtNoComponent();
const char* txtResistor();
const char* txtDiode();
const char* txtLED();
const char* txtCapacitor();
const char* txtUnknown();
const char* txtVoltmeterTitle();
const char* txtIRTitle();
const char* txtIRPrompt();
const char* txtHoldOKMenu();
const char* txtBatteryLow();
const char* txtExit();
void userActivity();
unsigned long autoOffIntervalMs();
bool autoOffTick();
void startSelectedStartupMode();


//======================================================
// CYRILLIC UI HELPERS
//======================================================
void drawLocalizedMenuItem(uint8_t index, int x, int y, uint16_t c);
void drawLocalizedSettingsItem(uint8_t index, int x, int y, uint16_t c);
void drawLocalizedMainMenuTitle(int cx, int y, uint16_t c);
void drawLocalizedSettingsTitle(int cx, int y, uint16_t c);
void drawLocalizedNoComponent(int cx, int y, uint16_t c);
void drawLocalizedLanguageTitle(int cx, int y, uint16_t c);
void drawLocalizedLanguageItem(uint8_t index, int x, int y, uint16_t c);

//======================================================
// VOLTMETER
//======================================================

void voltmeterLoop();

//======================================================
// IR REMOTE
//======================================================

void irRemoteLoop();
IRResult makeNoIR();
bool captureIRRaw(uint16_t *raw, uint8_t &count);
IRResult decodeNECFromRaw(uint16_t *raw, uint8_t count);
void drawNECResult(IRResult r, uint32_t lastCode, uint16_t packetCount);
void drawRAWResult(uint16_t *raw, uint8_t count, uint16_t packetCount);

//======================================================
// SETTINGS
//======================================================

void settingsLoop();

//======================================================
// ROBOT
//======================================================

void drawRobot(int x, int y, bool handUp);
void robotWave(int x, int y);

//======================================================
// BATTERY
//======================================================

float getBatteryVoltage();
int getBatteryPercent(float voltage);
void drawBattery(float voltage);
void drawSplashBattery();
void criticalBatteryShutdown(float voltage);

//======================================================
// DRAW COMPONENTS
//======================================================

uint16_t pinColor(uint8_t pin);
const char* pinName(uint8_t pin);

uint16_t resistorBodyColor(float r);

void drawResistorComponent(uint8_t pinA, uint8_t pinB, float resistance);
void drawDiodeComponent(uint8_t pinA, uint8_t pinB, float voltage, uint16_t triangleColor = TFT_YELLOW);
void drawCapacitorComponent(uint8_t pinA, uint8_t pinB, float capacitance, float esr, float vloss);

//======================================================
// TESTER
//======================================================

void testerLoop();
//======================================================
// COMPONENT CORE
//======================================================
//
// Единая структура результата определения.
// Её будут использовать:
// - Detect.ino
// - Tester.ino
// - DrawComponents.ino
//
//======================================================

//======================================================
// COMPONENT CORE
//======================================================
//
// Единое ядро результата определения компонента.
//
// Эту структуру будут использовать:
// - Detect.ino
// - Tester.ino
// - DrawComponents.ino
//
//======================================================

//------------------------------------------------------
// Типы компонентов
//------------------------------------------------------

enum ComponentType
{
  COMP_NONE = 0,       // ничего не найдено

  COMP_RESISTOR,       // резистор
  COMP_CAPACITOR,      // конденсатор
  COMP_DIODE,          // обычный диод
  COMP_LED,            // светодиод
  COMP_ZENER,          // стабилитрон

  COMP_NPN,            // NPN транзистор
  COMP_PNP,            // PNP транзистор

  COMP_NMOS,           // N-канальный MOSFET
  COMP_PMOS,           // P-канальный MOSFET

  COMP_JFET_N,         // N-JFET
  COMP_JFET_P,         // P-JFET

  COMP_THYRISTOR,      // тиристор
  COMP_TRIAC,          // симистор

  COMP_INDUCTOR,       // катушка

  COMP_UNKNOWN         // неизвестный компонент
};

//------------------------------------------------------
// Результат определения компонента
//------------------------------------------------------

struct ComponentResult
{
  ComponentType type;      // тип компонента

  // Выводы компонента
  uint8_t pinA;            // первый вывод
  uint8_t pinB;            // второй вывод
  uint8_t pinC;            // третий вывод

  // TRIAC: Nano определяет пару Gate-Main1, но не различает выводы внутри пары
  bool triacPairKnown;
  uint8_t triacPairPin1;
  uint8_t triacPairPin2;
  uint8_t triacMainPin;

  // Основные параметры
  float resistance;        // сопротивление, Ом
  float capacitance;       // ёмкость, uF
  float inductance;        // индуктивность, mH

  // Диоды / переходы
  float voltage;           // падение напряжения, В

  // Транзисторы
  float hFE;               // коэффициент усиления

  // MOSFET
  float gateThreshold;     // напряжение открытия затвора

  // Конденсаторы
  float esr;               // ESR, Ом
  float vloss;             // потеря напряжения, %

  // Дополнительные признаки
  bool reverse;            // обратная полярность
  bool enhancement;        // MOSFET enhancement mode
};

void drawComponent(ComponentResult c);
void drawTRIACComponent(ComponentResult c);
String formatResistance(float r);
String formatCapacitance(float capUF);

void drawTesterTopBar();
void drawTesterFrame();
void drawBJTComponent(ComponentResult c);
void drawMOSFETComponent(ComponentResult c);

//======================================================
// NANO UART MEASUREMENT HEAD
//======================================================

void nanoPowerInit();
void nanoPowerOn();
void nanoPowerOff();
bool nanoPowerState();

void nanoUartInit();
void nanoUartPoll();
void nanoTesterScreenInit();
void nanoRequestMeasurement();
void nanoResetAfterWake();
void nanoDrawResult();

// Cyrillic localized sound menu helpers
void drawLocalizedSoundTitle(int cx, int y, uint16_t c);
void drawLocalizedSoundItem(uint8_t itemIndex, int x, int y, uint16_t c);
void drawLocalizedOKSave(int cx, int y, uint16_t c);
void drawLocalizedSavedShort(int cx, int y, uint16_t c);

// Cyrillic localized auto off menu helpers
void drawLocalizedAutoOffTitle(int cx, int y, uint16_t c);
void drawLocalizedAutoOffItem(uint8_t itemIndex, int x, int y, uint16_t c);

// Cyrillic localized startup menu helpers
void drawLocalizedStartupTitle(int cx, int y, uint16_t c);
void drawLocalizedStartupItem(uint8_t index, int x, int y, uint16_t c);
void drawLocalizedTesterHint(int cx, int y, uint16_t c);
void drawLocalizedDisplayBrightnessTitle(int cx, int y, uint16_t c);
void drawLocalizedDisplayBrightnessItem(uint8_t index, int x, int y, uint16_t c);

// Boot splash marquee helpers
void bootMarqueeInit();
void bootMarqueeTick();

#endif
