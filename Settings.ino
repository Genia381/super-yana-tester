/****************************************************************
                YANA MULTITESTER
                File: Settings.ino

  Настройки прибора.

  Сохранение во Flash/EEPROM:
  - Startup Mode: Tester / Voltmeter
  - Sound Volume: Off / 25% / 50% / 75% / 100%
  - Auto Off: 1 min / 2 min / 5 min / Off
  - Display Brightness: 25% / 50% / 75% / 100%
****************************************************************/

#include "Config.h"

#if defined(ARDUINO_ARCH_RP2040) && __has_include(<hardware/flash.h>) && __has_include(<hardware/sync.h>)
  #include <hardware/flash.h>
  #include <hardware/sync.h>
  #define YANA_HAS_DIRECT_FLASH 1
#else
  #define YANA_HAS_DIRECT_FLASH 0
#endif

#if !YANA_HAS_DIRECT_FLASH
  #if __has_include(<EEPROM.h>)
    #include <EEPROM.h>
    #define YANA_HAS_EEPROM 1
  #else
    #define YANA_HAS_EEPROM 0
  #endif
#else
  #define YANA_HAS_EEPROM 0
#endif

//======================================================
// РАСПОЛОЖЕНИЕ SETTINGS
//======================================================

#define SETTINGS_TITLE_Y       18
#define SETTINGS_FIRST_Y       34
#define SETTINGS_STEP          16

#define SETTINGS_TEXT_X        32
#define SETTINGS_CURSOR_X      16

#define SETTINGS_SELECT_X      8
#define SETTINGS_SELECT_W      144
#define SETTINGS_SELECT_H      15

#define SETTINGS_CLEAR_X       2
#define SETTINGS_CLEAR_W       156
#define SETTINGS_CLEAR_H       17

//======================================================
// СОХРАНЕНИЕ НАСТРОЕК
//======================================================

#define SETTINGS_MAGIC32       0x59414E41UL   // 'YANA'
#define SETTINGS_VERSION       9
#define SETTINGS_PREV_VERSION  8
#define SETTINGS_OLD_VERSION   7
#define SETTINGS_OLDER_VERSION 6
#define SETTINGS_OLDEST_VERSION 5
#define SETTINGS_MODE_TESTER   0
#define SETTINGS_MODE_VOLT     1

#if YANA_HAS_DIRECT_FLASH
  #ifndef PICO_FLASH_SIZE_BYTES
    #define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
  #endif

  #define SETTINGS_FLASH_OFFSET   (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#endif

#if YANA_HAS_EEPROM
  #define SETTINGS_EEPROM_SIZE            512
  #define SETTINGS_EEPROM_ADDR_MAGIC0     0
  #define SETTINGS_EEPROM_ADDR_MAGIC1     1
  #define SETTINGS_EEPROM_ADDR_MAGIC2     2
  #define SETTINGS_EEPROM_ADDR_MAGIC3     3
  #define SETTINGS_EEPROM_ADDR_VERSION    4
  #define SETTINGS_EEPROM_ADDR_MODE       5
  #define SETTINGS_EEPROM_ADDR_VOLUME     6
  #define SETTINGS_EEPROM_ADDR_AUTO_OFF   7
  #define SETTINGS_EEPROM_ADDR_LANGUAGE   8
  #define SETTINGS_EEPROM_ADDR_BRIGHTNESS 9
  #define SETTINGS_EEPROM_ADDR_CHECKSUM   10
  #define SETTINGS_EEPROM_ADDR_VCAL0      16
  #define SETTINGS_EEPROM_ADDR_VCAL1      17
  #define SETTINGS_EEPROM_ADDR_VCAL2      18
  #define SETTINGS_EEPROM_ADDR_VCAL3      19
  #define SETTINGS_EEPROM_ADDR_SPLASH     20

  static bool settingsEepromReady = false;
#endif

StartupMode startupMode = STARTUP_TESTER;
uint8_t autoOffMode = 3;       // 0=1min, 1=2min, 2=5min, 3=Off
LanguageMode languageMode = LANG_EN;

// soundVolume хранится в Sound.ino

//======================================================
// Пункты Settings
//======================================================

const uint8_t SETTINGS_COUNT = 8;
const uint8_t SETTINGS_DISPLAY_INDEX = 0;
const uint8_t SETTINGS_SOUND_INDEX = 1;
const uint8_t SETTINGS_AUTO_OFF_INDEX = 2;
const uint8_t SETTINGS_CALIBRATION_INDEX = 3;
const uint8_t SETTINGS_STARTUP_INDEX = 4;
const uint8_t SETTINGS_SPLASH_INDEX = 5;
const uint8_t SETTINGS_LANGUAGE_INDEX = 6;
const uint8_t SETTINGS_EXIT_INDEX = 7;
const uint8_t SETTINGS_VISIBLE_COUNT = 4;

uint8_t settingsIndex = 0;
uint8_t oldSettingsIndex = 255;
uint8_t settingsTopIndex = 0;
uint8_t oldSettingsTopIndex = 255;


//======================================================
// LANGUAGE TEXT
// EN/RU/UA. Пока кириллицу не используем, чтобы TFT_eSPI
// не рисовал мусор. RU и UA сделаны короткой латиницей.
//======================================================

const char* txt3(const char* en, const char* ru, const char* ua)
{
  if (languageMode == LANG_RU) return ru;
  if (languageMode == LANG_UA) return ua;
  return en;
}

const char* txtExit() { return txt3("Exit", "Vyhod", "Vyhid"); }
const char* txtBack() { return txt3("Back", "Nazad", "Nazad"); }
const char* txtOKSelect() { return txt3("OK=Select", "OK=Vybor", "OK=Vybir"); }
const char* txtOKSave() { return txt3("OK=Save", "OK=Sohr.", "OK=Zber."); }
const char* txtOKBack() { return txt3("OK=Back", "OK=Nazad", "OK=Nazad"); }
const char* txtMainMenuHint() { return txt3("OK = Main Menu", "OK = Menu", "OK = Menu"); }
const char* txtSaved() { return txt3("Saved", "Sohraneno", "Zberezheno"); }
const char* txtComingSoon() { return txt3("Coming soon", "Poka pusto", "Piznishe"); }

const char* txtMainMenuTitle() { return txt3("MAIN MENU", "GLAVNOE MENU", "HOLOVNE MENU"); }
const char* txtSettingsTitle() { return txt3("SETTINGS", "NASTROIKI", "NALASHTUV."); }
const char* txtStartupTitle() { return txt3("STARTUP MODE", "REZHIM STARTA", "REZHYM STARTU"); }
const char* txtSoundTitle() { return txt3("SOUND", "ZVUK", "ZVUK"); }
const char* txtAutoOffTitle() { return txt3("AUTO OFF", "AVTO OTKL.", "AVTO VIMK."); }
const char* txtLanguageTitle() { return txt3("LANGUAGE", "YAZYK", "MOVA"); }
const char* txtCalibrationTitle() { return txt3("CALIBRATION", "KALIBROVKA", "KALIBRUV."); }

const char* txtNoComponent() { return txt3("NO COMPONENT", "NET DETALI", "NEMA DETALI"); }
const char* txtResistor() { return txt3("RESISTOR", "REZISTOR", "REZYSTOR"); }
const char* txtDiode() { return txt3("DIODE", "DIOD", "DIOD"); }
const char* txtLED() { return "LED"; }
const char* txtCapacitor() { return txt3("CAPACITOR", "KONDENSATOR", "KONDENSATOR"); }
const char* txtUnknown() { return txt3("UNKNOWN", "NEIZVESTNO", "NEVIDOMO"); }
const char* txtVoltmeterTitle() { return txt3("VOLTMETER", "VOLTMETR", "VOLTMETR"); }
const char* txtIRTitle() { return txt3("IR REMOTE", "IR PULT", "IR PULT"); }
const char* txtIRPrompt() { return txt3("Press remote key", "Nazmi knopku", "Natysny knopku"); }
const char* txtHoldOKMenu() { return txt3("HOLD OK=MENU", "UDERJ. OK=MENU", "UTRIM. OK=MENU"); }
const char* txtBatteryLow() { return txt3("BATTERY LOW", "BATAREYA RAZR.", "BATAREYA ROZR."); }

const char* txtMenuItem(uint8_t index)
{
  switch (index)
  {
    case 0: return txt3("Tester", "Tester", "Tester");
    case 1: return txt3("Probe", "Probnik", "Probnyk");
    case 2: return txt3("Voltmeter", "Voltmetr", "Voltmetr");
    case 3: return txt3("IR Remote", "IR Pult", "IR Pult");
    case 4: return txt3("Settings", "Nastroiki", "Nalasht.");
    case 5: return "Info";
    default: return txtExit();
  }
}

const char* txtSettingsItem(uint8_t index)
{
  switch (index)
  {
    case 0: return txt3("Display", "Ekran", "Ekran");
    case 1: return txt3("Sound", "Zvuk", "Zvuk");
    case 2: return "Auto Off";
    case 3: return txt3("Calibration", "Kalibrovka", "Kalibr.");
    case 4: return txt3("Startup Mode", "Start Rezhim", "Zapusk");
    case 5: return txt3("Splash", "Zastavka", "Zastavka");
    case 6: return txt3("Language", "Yazyk", "Mova");
    default: return txtExit();
  }
}

const char* txtStartupItem(uint8_t index)
{
  if (index == 0) return "Tester";
  if (index == 1) return txt3("Voltmeter", "Voltmetr", "Voltmetr");
  return txtExit();
}

const char* txtLanguageItem(uint8_t index)
{
  if (index == 0) return "EN";
  if (index == 1) return "RU";
  if (index == 2) return "UA";
  return txtExit();
}

//======================================================
// names
//======================================================

const char* startupModeName()
{
  return (startupMode == STARTUP_VOLTMETER) ? "Volt" : "Tester";
}

const char* autoOffName()
{
  switch (autoOffMode)
  {
    case 0: return "1m";
    case 1: return "2m";
    case 2: return "5m";
    default: return "Off";
  }
}

const char* languageName()
{
  if (languageMode == LANG_RU) return "RU";
  if (languageMode == LANG_UA) return "UA";
  return "EN";
}

//======================================================
// settings helpers
//======================================================

uint8_t settingsByteFromMode(StartupMode mode)
{
  return (mode == STARTUP_VOLTMETER) ? SETTINGS_MODE_VOLT : SETTINGS_MODE_TESTER;
}

StartupMode settingsModeFromByte(uint8_t mode)
{
  return (mode == SETTINGS_MODE_VOLT) ? STARTUP_VOLTMETER : STARTUP_TESTER;
}

uint8_t clampSoundVolume(uint8_t volume)
{
  if (volume > 4) return 4;
  return volume;
}

uint8_t clampAutoOffMode(uint8_t mode)
{
  if (mode > 3) return 3;
  return mode;
}

uint8_t clampLanguageMode(uint8_t mode)
{
  if (mode > 2) return 0;
  return mode;
}

uint8_t clampBootStyle(uint8_t style)
{
  if (style > 4) return 0;
  return style;
}

uint8_t settingsChecksum(uint8_t mode, uint8_t volume, uint8_t autoOff, uint8_t language, uint8_t brightness)
{
  return (uint8_t)(0xA5 ^ SETTINGS_VERSION ^ mode ^ volume ^ autoOff ^ language ^ brightness ^ 0x5A);
}

uint8_t settingsChecksumPrev7(uint8_t mode, uint8_t volume, uint8_t autoOff, uint8_t language)
{
  return (uint8_t)(0xA5 ^ SETTINGS_PREV_VERSION ^ mode ^ volume ^ autoOff ^ language ^ 0x5A);
}

uint8_t settingsChecksumOld6(uint8_t mode, uint8_t volume, uint8_t autoOff, uint8_t language)
{
  return (uint8_t)(0xA5 ^ SETTINGS_OLD_VERSION ^ mode ^ volume ^ autoOff ^ language ^ 0x5A);
}

uint8_t settingsChecksumOld5(uint8_t mode, uint8_t volume, uint8_t autoOff)
{
  return (uint8_t)(0xA5 ^ SETTINGS_OLDER_VERSION ^ mode ^ volume ^ autoOff ^ 0x5A);
}

uint8_t settingsChecksumOld4(uint8_t mode, uint8_t volume)
{
  return (uint8_t)(0xA5 ^ SETTINGS_OLDEST_VERSION ^ mode ^ volume ^ 0x5A);
}

//======================================================
// loadUserSettings()
//======================================================

void loadUserSettings()
{
#if YANA_HAS_DIRECT_FLASH
  const uint8_t *flashData = (const uint8_t *)(XIP_BASE + SETTINGS_FLASH_OFFSET);

  uint32_t magic = 0;
  memcpy(&magic, flashData + 0, sizeof(uint32_t));

  uint8_t version  = flashData[4];
  uint8_t mode     = flashData[5];
  uint8_t volume   = flashData[6];
  uint8_t autoOff  = flashData[7];
  uint8_t language = flashData[8];
  uint8_t brightness = flashData[9];
  uint8_t checksum = flashData[10];

  bool validNew = true;
  if (magic != SETTINGS_MAGIC32) validNew = false;
  if (version != SETTINGS_VERSION) validNew = false;
  if (mode > 1) validNew = false;
  if (volume > 4) validNew = false;
  if (autoOff > 3) validNew = false;
  if (language > 2) validNew = false;
  if (brightness > 3) validNew = false;
  if (checksum != settingsChecksum(mode, volume, autoOff, language, brightness)) validNew = false;

  if (validNew)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = (LanguageMode)clampLanguageMode(language);
    displayBrightness = clampDisplayBrightness(brightness);
    bootStyle = clampBootStyle(flashData[20]);
    float savedVCal = 1.0f;
    memcpy(&savedVCal, flashData + 16, sizeof(float));
    if (isfinite(savedVCal) && savedVCal >= 0.80f && savedVCal <= 1.20f)
      voltmeterCalibration = savedVCal;
    else
      voltmeterCalibration = 1.0f;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Совместимость с версией 7: языка уже был, яркости ещё не было.
  uint8_t checksumPrev7 = flashData[9];
  bool validPrev7 = true;
  if (magic != SETTINGS_MAGIC32) validPrev7 = false;
  if (version != SETTINGS_PREV_VERSION) validPrev7 = false;
  if (mode > 1) validPrev7 = false;
  if (volume > 4) validPrev7 = false;
  if (autoOff > 3) validPrev7 = false;
  if (language > 2) validPrev7 = false;
  if (checksumPrev7 != settingsChecksumPrev7(mode, volume, autoOff, language)) validPrev7 = false;

  if (validPrev7)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = (LanguageMode)clampLanguageMode(language);
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Совместимость с версией 6: RU/UA уже были, но EN ещё не было.
  uint8_t oldChecksum6 = flashData[9];
  bool validOld6 = true;
  if (magic != SETTINGS_MAGIC32) validOld6 = false;
  if (version != SETTINGS_OLD_VERSION) validOld6 = false;
  if (mode > 1) validOld6 = false;
  if (volume > 4) validOld6 = false;
  if (autoOff > 3) validOld6 = false;
  if (language > 1) validOld6 = false;
  if (oldChecksum6 != settingsChecksumOld6(mode, volume, autoOff, language)) validOld6 = false;

  if (validOld6)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = (language == 1) ? LANG_UA : LANG_RU;
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Совместимость с версией 5, где уже был Auto Off, но ещё не было языка.
  uint8_t oldChecksum5 = flashData[8];
  bool validOld5 = true;
  if (magic != SETTINGS_MAGIC32) validOld5 = false;
  if (version != SETTINGS_OLDER_VERSION) validOld5 = false;
  if (mode > 1) validOld5 = false;
  if (volume > 4) validOld5 = false;
  if (autoOff > 3) validOld5 = false;
  if (oldChecksum5 != settingsChecksumOld5(mode, volume, autoOff)) validOld5 = false;

  if (validOld5)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = LANG_RU;
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Совместимость с версией 4, где Auto Off и языка ещё не было.
  uint8_t oldChecksum4 = flashData[7];
  bool validOld4 = true;
  if (magic != SETTINGS_MAGIC32) validOld4 = false;
  if (version != SETTINGS_OLDEST_VERSION) validOld4 = false;
  if (mode > 1) validOld4 = false;
  if (volume > 4) validOld4 = false;
  if (oldChecksum4 != settingsChecksumOld4(mode, volume)) validOld4 = false;

  if (validOld4)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = 3;
    languageMode = LANG_RU;
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  startupMode = STARTUP_TESTER;
  soundVolume = 4;
  autoOffMode = 3;
  languageMode = LANG_EN;
  displayBrightness = 3;
  soundEnabled = true;
  applyDisplayBrightness();

#elif YANA_HAS_EEPROM
  if (!settingsEepromReady)
  {
    EEPROM.begin(SETTINGS_EEPROM_SIZE);
    settingsEepromReady = true;
  }

  uint32_t magic = 0;
  magic |= ((uint32_t)EEPROM.read(SETTINGS_EEPROM_ADDR_MAGIC0)) << 0;
  magic |= ((uint32_t)EEPROM.read(SETTINGS_EEPROM_ADDR_MAGIC1)) << 8;
  magic |= ((uint32_t)EEPROM.read(SETTINGS_EEPROM_ADDR_MAGIC2)) << 16;
  magic |= ((uint32_t)EEPROM.read(SETTINGS_EEPROM_ADDR_MAGIC3)) << 24;

  uint8_t version  = EEPROM.read(SETTINGS_EEPROM_ADDR_VERSION);
  uint8_t mode     = EEPROM.read(SETTINGS_EEPROM_ADDR_MODE);
  uint8_t volume   = EEPROM.read(SETTINGS_EEPROM_ADDR_VOLUME);
  uint8_t autoOff  = EEPROM.read(SETTINGS_EEPROM_ADDR_AUTO_OFF);
  uint8_t language = EEPROM.read(SETTINGS_EEPROM_ADDR_LANGUAGE);
  uint8_t brightness = EEPROM.read(SETTINGS_EEPROM_ADDR_BRIGHTNESS);
  uint8_t checksum = EEPROM.read(SETTINGS_EEPROM_ADDR_CHECKSUM);

  bool validNew = true;
  if (magic != SETTINGS_MAGIC32) validNew = false;
  if (version != SETTINGS_VERSION) validNew = false;
  if (mode > 1) validNew = false;
  if (volume > 4) validNew = false;
  if (autoOff > 3) validNew = false;
  if (language > 2) validNew = false;
  if (brightness > 3) validNew = false;
  if (checksum != settingsChecksum(mode, volume, autoOff, language, brightness)) validNew = false;

  if (validNew)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = (LanguageMode)clampLanguageMode(language);
    displayBrightness = clampDisplayBrightness(brightness);
    bootStyle = clampBootStyle(EEPROM.read(SETTINGS_EEPROM_ADDR_SPLASH));
    union { float f; uint8_t b[4]; } vcal;
    vcal.b[0] = EEPROM.read(SETTINGS_EEPROM_ADDR_VCAL0);
    vcal.b[1] = EEPROM.read(SETTINGS_EEPROM_ADDR_VCAL1);
    vcal.b[2] = EEPROM.read(SETTINGS_EEPROM_ADDR_VCAL2);
    vcal.b[3] = EEPROM.read(SETTINGS_EEPROM_ADDR_VCAL3);
    if (isfinite(vcal.f) && vcal.f >= 0.80f && vcal.f <= 1.20f)
      voltmeterCalibration = vcal.f;
    else
      voltmeterCalibration = 1.0f;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Старая структура v8: языка уже был, яркости ещё не было.
  uint8_t checksumPrev7Eeprom = EEPROM.read(9);
  bool validPrev7Eeprom = true;
  if (magic != SETTINGS_MAGIC32) validPrev7Eeprom = false;
  if (version != SETTINGS_PREV_VERSION) validPrev7Eeprom = false;
  if (mode > 1) validPrev7Eeprom = false;
  if (volume > 4) validPrev7Eeprom = false;
  if (autoOff > 3) validPrev7Eeprom = false;
  if (language > 2) validPrev7Eeprom = false;
  if (checksumPrev7Eeprom != settingsChecksumPrev7(mode, volume, autoOff, language)) validPrev7Eeprom = false;

  if (validPrev7Eeprom)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = (LanguageMode)clampLanguageMode(language);
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Старая структура v6: RU/UA уже были, но EN ещё не было.
  uint8_t oldChecksum6Eeprom = EEPROM.read(9);
  bool validOld6 = true;
  if (magic != SETTINGS_MAGIC32) validOld6 = false;
  if (version != SETTINGS_OLD_VERSION) validOld6 = false;
  if (mode > 1) validOld6 = false;
  if (volume > 4) validOld6 = false;
  if (autoOff > 3) validOld6 = false;
  if (language > 1) validOld6 = false;
  if (oldChecksum6Eeprom != settingsChecksumOld6(mode, volume, autoOff, language)) validOld6 = false;

  if (validOld6)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = (language == 1) ? LANG_UA : LANG_RU;
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Старая структура v5: checksum лежал в байте 8, языка ещё не было.
  uint8_t oldChecksum5 = EEPROM.read(8);
  bool validOld5 = true;
  if (magic != SETTINGS_MAGIC32) validOld5 = false;
  if (version != SETTINGS_OLDER_VERSION) validOld5 = false;
  if (mode > 1) validOld5 = false;
  if (volume > 4) validOld5 = false;
  if (autoOff > 3) validOld5 = false;
  if (oldChecksum5 != settingsChecksumOld5(mode, volume, autoOff)) validOld5 = false;

  if (validOld5)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = clampAutoOffMode(autoOff);
    languageMode = LANG_RU;
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  // Старая структура v4: checksum лежал в байте 7, Auto Off не было.
  uint8_t oldChecksum4 = EEPROM.read(7);
  bool validOld4 = true;
  if (magic != SETTINGS_MAGIC32) validOld4 = false;
  if (version != SETTINGS_OLDEST_VERSION) validOld4 = false;
  if (mode > 1) validOld4 = false;
  if (volume > 4) validOld4 = false;
  if (oldChecksum4 != settingsChecksumOld4(mode, volume)) validOld4 = false;

  if (validOld4)
  {
    startupMode = settingsModeFromByte(mode);
    soundVolume = clampSoundVolume(volume);
    autoOffMode = 3;
    languageMode = LANG_RU;
    displayBrightness = 3;
    soundEnabled = (soundVolume > 0);
    applyDisplayBrightness();
    return;
  }

  startupMode = STARTUP_TESTER;
  soundVolume = 4;
  autoOffMode = 3;
  languageMode = LANG_EN;
  displayBrightness = 3;
  soundEnabled = true;
  applyDisplayBrightness();
#else
  startupMode = STARTUP_TESTER;
  soundVolume = 4;
  autoOffMode = 3;
  languageMode = LANG_EN;
  displayBrightness = 3;
  soundEnabled = true;
  applyDisplayBrightness();
#endif
}

//======================================================
// saveUserSettings()
//======================================================

void saveUserSettings()
{
  uint8_t m = settingsByteFromMode(startupMode);
  uint8_t v = clampSoundVolume(soundVolume);
  uint8_t a = clampAutoOffMode(autoOffMode);
  uint8_t l = clampLanguageMode((uint8_t)languageMode);
  uint8_t b = clampDisplayBrightness(displayBrightness);
  uint8_t s = clampBootStyle(bootStyle);

#if YANA_HAS_DIRECT_FLASH
  uint8_t page[FLASH_PAGE_SIZE];

  for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++)
    page[i] = 0xFF;

  uint32_t magic = SETTINGS_MAGIC32;
  memcpy(page + 0, &magic, sizeof(uint32_t));

  page[4] = SETTINGS_VERSION;
  page[5] = m;
  page[6] = v;
  page[7] = a;
  page[8] = l;
  page[9] = b;
  page[10] = settingsChecksum(m, v, a, l, b);
  page[11] = (uint8_t)(m ^ 0xFF);
  page[12] = (uint8_t)(v ^ 0xFF);
  page[13] = (uint8_t)(a ^ 0xFF);
  page[14] = (uint8_t)(l ^ 0xFF);
  page[15] = (uint8_t)(b ^ 0xFF);
  memcpy(page + 16, &voltmeterCalibration, sizeof(float));
  page[20] = s;

  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(SETTINGS_FLASH_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(SETTINGS_FLASH_OFFSET, page, FLASH_PAGE_SIZE);
  restore_interrupts(ints);

#elif YANA_HAS_EEPROM
  if (!settingsEepromReady)
  {
    EEPROM.begin(SETTINGS_EEPROM_SIZE);
    settingsEepromReady = true;
  }

  EEPROM.write(SETTINGS_EEPROM_ADDR_MAGIC0, (uint8_t)((SETTINGS_MAGIC32 >> 0) & 0xFF));
  EEPROM.write(SETTINGS_EEPROM_ADDR_MAGIC1, (uint8_t)((SETTINGS_MAGIC32 >> 8) & 0xFF));
  EEPROM.write(SETTINGS_EEPROM_ADDR_MAGIC2, (uint8_t)((SETTINGS_MAGIC32 >> 16) & 0xFF));
  EEPROM.write(SETTINGS_EEPROM_ADDR_MAGIC3, (uint8_t)((SETTINGS_MAGIC32 >> 24) & 0xFF));
  EEPROM.write(SETTINGS_EEPROM_ADDR_VERSION, SETTINGS_VERSION);
  EEPROM.write(SETTINGS_EEPROM_ADDR_MODE, m);
  EEPROM.write(SETTINGS_EEPROM_ADDR_VOLUME, v);
  EEPROM.write(SETTINGS_EEPROM_ADDR_AUTO_OFF, a);
  EEPROM.write(SETTINGS_EEPROM_ADDR_LANGUAGE, l);
  EEPROM.write(SETTINGS_EEPROM_ADDR_BRIGHTNESS, b);
  EEPROM.write(SETTINGS_EEPROM_ADDR_CHECKSUM, settingsChecksum(m, v, a, l, b));
  union { float f; uint8_t b[4]; } vcal;
  vcal.f = voltmeterCalibration;
  EEPROM.write(SETTINGS_EEPROM_ADDR_VCAL0, vcal.b[0]);
  EEPROM.write(SETTINGS_EEPROM_ADDR_VCAL1, vcal.b[1]);
  EEPROM.write(SETTINGS_EEPROM_ADDR_VCAL2, vcal.b[2]);
  EEPROM.write(SETTINGS_EEPROM_ADDR_VCAL3, vcal.b[3]);
  EEPROM.write(SETTINGS_EEPROM_ADDR_SPLASH, s);
  EEPROM.commit();
#endif
}

//======================================================
// save wrappers
//======================================================

void saveStartupMode(StartupMode mode)
{
  startupMode = mode;
  saveUserSettings();
}

void saveSoundVolume(uint8_t volume)
{
  soundVolume = clampSoundVolume(volume);
  soundEnabled = (soundVolume > 0);
  saveUserSettings();
}

void saveAutoOffMode(uint8_t mode)
{
  autoOffMode = clampAutoOffMode(mode);
  saveUserSettings();
}

void saveLanguageMode(LanguageMode mode)
{
  languageMode = (LanguageMode)clampLanguageMode((uint8_t)mode);
  saveUserSettings();
}

//======================================================
// startSelectedStartupMode()
//======================================================

void startSelectedStartupMode()
{
  userActivity();

  if (startupMode == STARTUP_VOLTMETER)
    voltmeterLoop();
  else
    testerLoop();
}

//======================================================
// drawOneSettingsItem()
//======================================================

void drawOneSettingsItem(uint8_t i, bool selected)
{
  int y = SETTINGS_FIRST_Y + (i - settingsTopIndex) * SETTINGS_STEP;

  tft.fillRect(SETTINGS_CLEAR_X, y - 1, SETTINGS_CLEAR_W, SETTINGS_CLEAR_H, COLOR_BG);

  if (selected)
  {
    tft.fillRect(SETTINGS_SELECT_X, y, SETTINGS_SELECT_W, SETTINGS_SELECT_H, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", SETTINGS_CURSOR_X, y, 2);
    drawLocalizedSettingsItem(i, SETTINGS_TEXT_X, y, (i == SETTINGS_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (i == SETTINGS_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedSettingsItem(i, SETTINGS_TEXT_X, y, textColor);
  }

  if (i == SETTINGS_DISPLAY_INDEX)
  {
    tft.setTextColor(selected ? TFT_YELLOW : TFT_CYAN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawRightString(displayBrightnessName(), 150, y, 1);
  }

  if (i == SETTINGS_SOUND_INDEX)
  {
    tft.setTextColor(selected ? TFT_YELLOW : TFT_CYAN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawRightString(soundVolumeName(), 150, y, 1);
  }

  if (i == SETTINGS_AUTO_OFF_INDEX)
  {
    tft.setTextColor(selected ? TFT_YELLOW : TFT_CYAN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawRightString(autoOffName(), 150, y, 1);
  }

  if (i == SETTINGS_STARTUP_INDEX)
  {
    tft.setTextColor(selected ? TFT_YELLOW : TFT_CYAN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawRightString(startupModeName(), 150, y, 1);
  }

  if (i == SETTINGS_SPLASH_INDEX)
  {
    tft.setTextColor(selected ? TFT_YELLOW : TFT_CYAN, selected ? TFT_BLUE : COLOR_BG);
    String splashValue = String((int)bootStyle + 1);
    tft.drawRightString(splashValue, 150, y, 1);
  }

  if (i == SETTINGS_LANGUAGE_INDEX)
  {
    tft.setTextColor(selected ? TFT_YELLOW : TFT_CYAN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawRightString(languageName(), 150, y, 1);
  }
}

//======================================================
// drawSettings()
//======================================================

void keepSettingsVisible()
{
  if (settingsIndex < settingsTopIndex) settingsTopIndex = settingsIndex;
  if (settingsIndex >= settingsTopIndex + SETTINGS_VISIBLE_COUNT)
    settingsTopIndex = settingsIndex - SETTINGS_VISIBLE_COUNT + 1;

  if (settingsTopIndex + SETTINGS_VISIBLE_COUNT > SETTINGS_COUNT)
    settingsTopIndex = SETTINGS_COUNT - SETTINGS_VISIBLE_COUNT;
}

void drawSettingsItems()
{
  keepSettingsVisible();

  tft.fillRect(1, 31, 158, 80, COLOR_BG);

  for (uint8_t line = 0; line < SETTINGS_VISIBLE_COUNT; line++)
  {
    uint8_t item = settingsTopIndex + line;
    if (item < SETTINGS_COUNT) drawOneSettingsItem(item, item == settingsIndex);
  }

  tft.fillRect(150, 32, 8, 76, COLOR_BG);
  tft.setTextColor(TFT_CYAN, COLOR_BG);
  if (settingsTopIndex > 0) tft.drawString("^", 151, 38, 1);
  if (settingsTopIndex + SETTINGS_VISIBLE_COUNT < SETTINGS_COUNT) tft.drawString("v", 151, 100, 1);

  oldSettingsIndex = settingsIndex;
  oldSettingsTopIndex = settingsTopIndex;
}

void drawSettings(bool fullRedraw)
{
  keepSettingsVisible();

  if (fullRedraw)
  {
    clearScreen();
    drawStatusBar();
    drawFrame();
    drawBottomBar();

    tft.fillRect(1, 17, 158, 94, COLOR_BG);

    tft.setTextColor(TFT_YELLOW, COLOR_BG);
    drawLocalizedSettingsTitle(CENTER_X, SETTINGS_TITLE_Y, TFT_YELLOW);

    drawSettingsItems();

    tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
    drawLocalizedOKSelectHint(CENTER_X, 116, TFT_GREEN);

    return;
  }

  if (oldSettingsIndex != settingsIndex || oldSettingsTopIndex != settingsTopIndex)
  {
    drawSettingsItems();
  }
}

//======================================================
// DISPLAY BRIGHTNESS SUBMENU
//======================================================

const uint8_t DISPLAY_BRIGHTNESS_COUNT = 5;
const uint8_t DISPLAY_BRIGHTNESS_EXIT_INDEX = 4;
const uint8_t DISPLAY_BRIGHTNESS_VISIBLE_COUNT = 4;
const int DISPLAY_BRIGHTNESS_FIRST_Y = 40;
const int DISPLAY_BRIGHTNESS_STEP = 16;
uint8_t displayBrightnessTopIndex = 0;

void updateDisplayBrightnessScroll(uint8_t selected)
{
  if (selected < displayBrightnessTopIndex)
    displayBrightnessTopIndex = selected;

  if (selected >= displayBrightnessTopIndex + DISPLAY_BRIGHTNESS_VISIBLE_COUNT)
    displayBrightnessTopIndex = selected - DISPLAY_BRIGHTNESS_VISIBLE_COUNT + 1;

  if (displayBrightnessTopIndex + DISPLAY_BRIGHTNESS_VISIBLE_COUNT > DISPLAY_BRIGHTNESS_COUNT)
  {
    if (DISPLAY_BRIGHTNESS_COUNT > DISPLAY_BRIGHTNESS_VISIBLE_COUNT)
      displayBrightnessTopIndex = DISPLAY_BRIGHTNESS_COUNT - DISPLAY_BRIGHTNESS_VISIBLE_COUNT;
    else
      displayBrightnessTopIndex = 0;
  }
}

void drawDisplayBrightnessScrollMarks()
{
  tft.fillRect(150, 36, 8, 72, COLOR_BG);
  tft.setTextColor(TFT_CYAN, COLOR_BG);

  if (displayBrightnessTopIndex > 0)
    tft.drawString("^", 151, 38, 1);

  if (displayBrightnessTopIndex + DISPLAY_BRIGHTNESS_VISIBLE_COUNT < DISPLAY_BRIGHTNESS_COUNT)
    tft.drawString("v", 151, 100, 1);
}

void drawDisplayBrightnessItem(uint8_t screenLine, uint8_t index, bool selected)
{
  int y = DISPLAY_BRIGHTNESS_FIRST_Y + screenLine * DISPLAY_BRIGHTNESS_STEP;
  tft.fillRect(SETTINGS_CLEAR_X, y - 1, SETTINGS_CLEAR_W - 8, SETTINGS_SELECT_H + 1, COLOR_BG);

  if (selected)
  {
    tft.fillRect(SETTINGS_SELECT_X, y, SETTINGS_SELECT_W, SETTINGS_SELECT_H, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", SETTINGS_CURSOR_X, y + 1, 2);
    drawLocalizedDisplayBrightnessItem(index, SETTINGS_TEXT_X, y + 1, (index == DISPLAY_BRIGHTNESS_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (index == DISPLAY_BRIGHTNESS_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedDisplayBrightnessItem(index, SETTINGS_TEXT_X, y + 1, textColor);
  }

  if (index == displayBrightness && index < DISPLAY_BRIGHTNESS_EXIT_INDEX)
  {
    tft.setTextColor(TFT_GREEN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawString("*", 132, y + 1, 2);
  }
}

void drawDisplayBrightnessItems(uint8_t selected)
{
  updateDisplayBrightnessScroll(selected);
  tft.fillRect(SETTINGS_CLEAR_X, DISPLAY_BRIGHTNESS_FIRST_Y - 4, SETTINGS_CLEAR_W, 70, COLOR_BG);

  for (uint8_t line = 0; line < DISPLAY_BRIGHTNESS_VISIBLE_COUNT; line++)
  {
    uint8_t itemIndex = displayBrightnessTopIndex + line;
    if (itemIndex >= DISPLAY_BRIGHTNESS_COUNT) break;
    drawDisplayBrightnessItem(line, itemIndex, selected == itemIndex);
  }

  drawDisplayBrightnessScrollMarks();
}

void drawDisplayBrightnessMenu(uint8_t selected)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  drawLocalizedDisplayBrightnessTitle(CENTER_X, 18, TFT_YELLOW);

  drawDisplayBrightnessItems(selected);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSave(CENTER_X, 116, TFT_GREEN);
}

void displayBrightnessLoop()
{
  // При входе показываем четыре пункта: 25%, 50%, 75%, 100%.
  // Выход уходит ниже в скроллинг.
  uint8_t selected = clampDisplayBrightness(displayBrightness);
  uint8_t oldSelected = selected;
  displayBrightnessTopIndex = 0;

  // При входе сразу показываем сохранённую ступень яркости.
  // Теперь это безопасно: PWM нет, только GPIO через резисторы.
  applyDisplayBrightness();
  drawDisplayBrightnessMenu(selected);

  while (true)
  {
    if (autoOffTick()) drawDisplayBrightnessMenu(selected);

    if (buttonUp()) selected = (selected == 0) ? DISPLAY_BRIGHTNESS_COUNT - 1 : selected - 1;
    if (buttonDown()) { selected++; if (selected >= DISPLAY_BRIGHTNESS_COUNT) selected = 0; }

    if (oldSelected != selected)
    {
      // При прокрутке сразу показываем яркость.
      // Если выбран Выход, возвращаем сохранённую яркость.
      if (selected < DISPLAY_BRIGHTNESS_EXIT_INDEX) previewDisplayBrightness(selected);
      else applyDisplayBrightness();
      drawDisplayBrightnessMenu(selected);
      oldSelected = selected;
    }

    if (buttonOK())
    {
      if (selected == DISPLAY_BRIGHTNESS_EXIT_INDEX)
      {
        applyDisplayBrightness();
        drawSettings(true);
        return;
      }

      // OK сохраняет выбранную яркость в память и оставляет её включённой.
      saveDisplayBrightness(selected);
      soundSaved();
      tft.fillRect(1, 91, 158, 20, COLOR_BG);
      tft.setTextColor(TFT_GREEN, COLOR_BG);
      drawLocalizedSavedShort(CENTER_X, 92, TFT_GREEN);
      delay(600);
      userActivity();
      drawDisplayBrightnessMenu(selected);
    }

    delay(5);
  }
}

//======================================================
// STARTUP MODE SUBMENU
//======================================================

const uint8_t STARTUP_COUNT = 3;
const uint8_t STARTUP_EXIT_INDEX = 2;

void drawStartupModeItem(uint8_t index, bool selected)
{
  int y = 38 + index * 18;
  tft.fillRect(8, y - 2, 144, 20, COLOR_BG);

  if (selected)
  {
    tft.fillRect(8, y - 1, 144, 18, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", 18, y, 2);
    drawLocalizedStartupItem(index, 34, y, TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (index == STARTUP_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedStartupItem(index, 34, y, textColor);
  }

  bool active = false;
  if (index == 0 && startupMode == STARTUP_TESTER) active = true;
  if (index == 1 && startupMode == STARTUP_VOLTMETER) active = true;

  if (active)
  {
    tft.setTextColor(TFT_GREEN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawString("*", 132, y, 2);
  }
}

void drawStartupModeMenu(uint8_t selected)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  drawLocalizedStartupTitle(CENTER_X, 22, TFT_YELLOW);

  for (uint8_t i = 0; i < STARTUP_COUNT; i++)
    drawStartupModeItem(i, selected == i);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSelectHint(CENTER_X, 116, TFT_GREEN);
}

void startupModeLoop()
{
  uint8_t selected = (startupMode == STARTUP_VOLTMETER) ? 1 : 0;
  uint8_t oldSelected = 255;
  drawStartupModeMenu(selected);

  while (true)
  {
    if (autoOffTick()) drawStartupModeMenu(selected);

    if (buttonUp()) selected = (selected == 0) ? STARTUP_COUNT - 1 : selected - 1;
    if (buttonDown()) { selected++; if (selected >= STARTUP_COUNT) selected = 0; }

    if (oldSelected != selected)
    {
      drawStartupModeMenu(selected);
      oldSelected = selected;
    }

    if (buttonOK())
    {
      if (selected == STARTUP_EXIT_INDEX)
      {
        drawSettings(true);
        return;
      }

      if (selected == 0) saveStartupMode(STARTUP_TESTER);
      else saveStartupMode(STARTUP_VOLTMETER);

      soundSaved();
      tft.fillRect(1, 91, 158, 20, COLOR_BG);
      tft.setTextColor(TFT_GREEN, COLOR_BG);
      drawLocalizedSavedShort(CENTER_X, 92, TFT_GREEN);
      delay(600);
      userActivity();
      drawStartupModeMenu(selected);
    }

    delay(5);
  }
}

//======================================================
// SOUND VOLUME SUBMENU
//======================================================

const char *soundVolumeItems[] = { "Off", "25%", "50%", "75%", "100%", "Exit" };
const uint8_t SOUND_VOLUME_COUNT = 6;
const uint8_t SOUND_VOLUME_EXIT_INDEX = 5;
const uint8_t SOUND_VOLUME_VISIBLE = 4;
uint8_t soundVolumeTop = 0;

void updateSoundVolumeScroll(uint8_t selected)
{
  if (selected < soundVolumeTop) soundVolumeTop = selected;
  if (selected >= soundVolumeTop + SOUND_VOLUME_VISIBLE) soundVolumeTop = selected - SOUND_VOLUME_VISIBLE + 1;
  if (soundVolumeTop + SOUND_VOLUME_VISIBLE > SOUND_VOLUME_COUNT) soundVolumeTop = SOUND_VOLUME_COUNT - SOUND_VOLUME_VISIBLE;
}

void drawSoundVolumeItem(uint8_t screenLine, uint8_t itemIndex, bool selected)
{
  int y = 42 + screenLine * 16;
  tft.fillRect(8, y - 2, 144, 17, COLOR_BG);

  if (selected)
  {
    tft.fillRect(8, y - 1, 144, 16, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", 18, y, 2);
    drawLocalizedSoundItem(itemIndex, 34, y, (itemIndex == SOUND_VOLUME_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (itemIndex == SOUND_VOLUME_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedSoundItem(itemIndex, 34, y, textColor);
  }

  if (itemIndex == soundVolume && itemIndex < SOUND_VOLUME_EXIT_INDEX)
  {
    tft.setTextColor(TFT_GREEN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawString("*", 132, y, 2);
  }
}

void drawSoundVolumeMenu(uint8_t selected)
{
  updateSoundVolumeScroll(selected);
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  drawLocalizedSoundTitle(CENTER_X, 22, TFT_YELLOW);

  for (uint8_t line = 0; line < SOUND_VOLUME_VISIBLE; line++)
  {
    uint8_t item = soundVolumeTop + line;
    drawSoundVolumeItem(line, item, selected == item);
  }

  tft.fillRect(150, 38, 8, 70, COLOR_BG);
  tft.setTextColor(TFT_CYAN, COLOR_BG);
  if (soundVolumeTop > 0) tft.drawString("^", 151, 40, 1);
  if (soundVolumeTop + SOUND_VOLUME_VISIBLE < SOUND_VOLUME_COUNT) tft.drawString("v", 151, 100, 1);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSave(CENTER_X, 116, TFT_GREEN);
}

void soundVolumeLoop()
{
  uint8_t selected = soundVolume;
  if (selected > 4) selected = 4;
  uint8_t oldSelected = 255;
  soundVolumeTop = 0;
  drawSoundVolumeMenu(selected);

  while (true)
  {
    if (autoOffTick()) drawSoundVolumeMenu(selected);

    if (buttonUp()) selected = (selected == 0) ? SOUND_VOLUME_COUNT - 1 : selected - 1;
    if (buttonDown()) { selected++; if (selected >= SOUND_VOLUME_COUNT) selected = 0; }

    if (oldSelected != selected)
    {
      drawSoundVolumeMenu(selected);
      oldSelected = selected;
    }

    if (buttonOK())
    {
      if (selected == SOUND_VOLUME_EXIT_INDEX)
      {
        drawSettings(true);
        return;
      }

      saveSoundVolume(selected);
      soundSaved();
      tft.fillRect(1, 91, 158, 20, COLOR_BG);
      tft.setTextColor(TFT_GREEN, COLOR_BG);
      drawLocalizedSavedShort(CENTER_X, 92, TFT_GREEN);
      delay(600);
      userActivity();
      drawSoundVolumeMenu(selected);
    }

    delay(5);
  }
}

//======================================================
// AUTO OFF SUBMENU
//======================================================

const char *autoOffItems[] = { "1 min", "2 min", "5 min", "Off", "Exit" };
const uint8_t AUTO_OFF_COUNT = 5;
const uint8_t AUTO_OFF_EXIT_INDEX = 4;
const uint8_t AUTO_OFF_VISIBLE = 4;
uint8_t autoOffTop = 0;

void keepAutoOffVisible(uint8_t selected)
{
  if (selected < autoOffTop) autoOffTop = selected;
  if (selected >= autoOffTop + AUTO_OFF_VISIBLE) autoOffTop = selected - AUTO_OFF_VISIBLE + 1;
  if (autoOffTop > AUTO_OFF_COUNT - AUTO_OFF_VISIBLE) autoOffTop = AUTO_OFF_COUNT - AUTO_OFF_VISIBLE;
}

void drawAutoOffVisibleItem(uint8_t slot, uint8_t index, bool selected)
{
  int y = 38 + slot * 16;
  tft.fillRect(8, y - 2, 144, 17, COLOR_BG);

  if (selected)
  {
    tft.fillRect(8, y - 1, 144, 16, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", 18, y, 2);
    drawLocalizedAutoOffItem(index, 34, y, (index == AUTO_OFF_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (index == AUTO_OFF_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedAutoOffItem(index, 34, y, (index == AUTO_OFF_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }

  if (index == autoOffMode && index < AUTO_OFF_EXIT_INDEX)
  {
    tft.setTextColor(TFT_GREEN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawString("*", 132, y, 2);
  }
}

void drawAutoOffMenu(uint8_t selected)
{
  keepAutoOffVisible(selected);

  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  drawLocalizedAutoOffTitle(CENTER_X, 20, TFT_YELLOW);

  for (uint8_t slot = 0; slot < AUTO_OFF_VISIBLE; slot++)
  {
    uint8_t index = autoOffTop + slot;
    if (index < AUTO_OFF_COUNT)
      drawAutoOffVisibleItem(slot, index, selected == index);
  }

  tft.setTextColor(TFT_CYAN, COLOR_BG);
  if (autoOffTop > 0) tft.drawString("^", 151, 40, 1);
  if (autoOffTop + AUTO_OFF_VISIBLE < AUTO_OFF_COUNT) tft.drawString("v", 151, 100, 1);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSave(CENTER_X, 116, TFT_GREEN);
}

void autoOffLoop()
{
  uint8_t selected = autoOffMode;
  if (selected > 3) selected = 3;
  autoOffTop = 0;
  keepAutoOffVisible(selected);
  uint8_t oldSelected = 255;
  drawAutoOffMenu(selected);

  while (true)
  {
    if (autoOffTick()) drawAutoOffMenu(selected);

    if (buttonUp()) selected = (selected == 0) ? AUTO_OFF_COUNT - 1 : selected - 1;
    if (buttonDown()) { selected++; if (selected >= AUTO_OFF_COUNT) selected = 0; }

    if (oldSelected != selected)
    {
      drawAutoOffMenu(selected);
      oldSelected = selected;
    }

    if (buttonOK())
    {
      if (selected == AUTO_OFF_EXIT_INDEX)
      {
        drawSettings(true);
        return;
      }

      saveAutoOffMode(selected);
      soundSaved();
      tft.fillRect(1, 91, 158, 20, COLOR_BG);
      tft.setTextColor(TFT_GREEN, COLOR_BG);
      drawLocalizedSavedShort(CENTER_X, 92, TFT_GREEN);
      delay(600);
      userActivity();
      drawAutoOffMenu(selected);
    }

    delay(5);
  }
}



//======================================================
// МЕНЮ КАЛИБРОВКИ
//
// Безопасный тестовый мастер калибровки.
// До окончательной сборки данные не записываются.
//======================================================

const uint8_t CALIBRATION_COUNT = 4;
const uint8_t CALIBRATION_NANO_INDEX = 0;
const uint8_t CALIBRATION_VOLTMETER_INDEX = 1;
const uint8_t CALIBRATION_STATUS_INDEX = 2;
const uint8_t CALIBRATION_BACK_INDEX = 3;

static void drawCalibrationTitle()
{
  if (languageMode == LANG_RU)
    drawCyrTextCentered("KALIBROVKA", CENTER_X, 23, TFT_YELLOW);
  else if (languageMode == LANG_UA)
    drawCyrTextCentered("KALiBRUV.", CENTER_X, 23, TFT_YELLOW);
  else
    tft.drawCentreString("CALIBRATION", CENTER_X, 20, 2);
}

static void drawCalibrationItemText(uint8_t index, int x, int y, uint16_t color)
{
  if (languageMode == LANG_RU)
  {
    if (index == CALIBRATION_NANO_INDEX) drawCyrText("TESTER NANO", x, y + 3, color);
    else if (index == CALIBRATION_VOLTMETER_INDEX) drawCyrText("VOLbTMETR", x, y + 3, color);
    else if (index == CALIBRATION_STATUS_INDEX) drawCyrText("SOSTOQNIE", x, y + 3, color);
    else drawCyrText("NAZAD", x, y + 3, TFT_RED);
    return;
  }

  if (languageMode == LANG_UA)
  {
    if (index == CALIBRATION_NANO_INDEX) drawCyrText("TESTER NANO", x, y + 3, color);
    else if (index == CALIBRATION_VOLTMETER_INDEX) drawCyrText("VOLbTMETR", x, y + 3, color);
    else if (index == CALIBRATION_STATUS_INDEX) drawCyrText("STAN", x, y + 3, color);
    else drawCyrText("NAZAD", x, y + 3, TFT_RED);
    return;
  }

  const char* s = "BACK";
  if (index == CALIBRATION_NANO_INDEX) s = "NANO TESTER";
  else if (index == CALIBRATION_VOLTMETER_INDEX) s = "VOLTMETER";
  else if (index == CALIBRATION_STATUS_INDEX) s = "STATUS";
  tft.drawString(s, x, y, 2);
}

static void drawCalibrationMenu(uint8_t selected)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  drawCalibrationTitle();

  for (uint8_t i = 0; i < CALIBRATION_COUNT; i++)
  {
    int y = 38 + i * 17;
    bool active = (i == selected);
    uint16_t color = (i == CALIBRATION_BACK_INDEX) ? TFT_RED : TFT_WHITE;

    if (active)
    {
      tft.fillRoundRect(8, y - 1, 144, 16, 4, TFT_BLUE);
      tft.setTextColor(color, TFT_BLUE);
      tft.drawString(">", 16, y, 2);
      drawCalibrationItemText(i, 31, y, color);
    }
    else
    {
      tft.setTextColor(color, COLOR_BG);
      drawCalibrationItemText(i, 31, y, color);
    }
  }

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSelectHint(CENTER_X, 116, TFT_GREEN);
}

static void drawCalibrationConfirmScreen(bool nanoTester, bool holding, uint16_t heldMs)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  if (nanoTester)
    tft.drawCentreString("NANO TESTER", CENTER_X, 21, 2);
  else
    drawVoltmeterTitle();

  tft.fillRoundRect(10, 40, 140, 54, 7, 0x0186);
  tft.drawRoundRect(10, 40, 140, 54, 7, TFT_CYAN);

  if (languageMode == LANG_RU)
  {
    drawCyrTextCentered("TESTOVYJ REJIM", CENTER_X, 47, TFT_GREEN);
    drawCyrTextCentered("BEZ ZAPISI", CENTER_X, 61, TFT_WHITE);
    drawCyrTextCentered("UDERJ. OK", CENTER_X, 76, TFT_YELLOW);
  }
  else if (languageMode == LANG_UA)
  {
    drawCyrTextCentered("TESTOVIJ REJIM", CENTER_X, 47, TFT_GREEN);
    drawCyrTextCentered("BEZ ZAPISU", CENTER_X, 61, TFT_WHITE);
    drawCyrTextCentered("UTRIM. OK", CENTER_X, 76, TFT_YELLOW);
  }
  else
  {
    tft.setTextColor(TFT_GREEN, 0x0186);
    tft.drawCentreString("TEST MODE", CENTER_X, 47, 2);
    tft.setTextColor(TFT_WHITE, 0x0186);
    tft.drawCentreString("NO DATA WRITE", CENTER_X, 62, 1);
    tft.setTextColor(TFT_YELLOW, 0x0186);
    tft.drawCentreString("HOLD OK", CENTER_X, 76, 1);
  }

  // Полоса удержания OK
  tft.drawRect(22, 86, 116, 6, TFT_WHITE);
  int fill = map((int)heldMs, 0, 1800, 0, 112);
  if (fill < 0) fill = 0;
  if (fill > 112) fill = 112;
  if (holding && fill > 0) tft.fillRect(24, 88, fill, 2, TFT_GREEN);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  if (languageMode == LANG_RU)
    drawCyrTextCentered("KOROTKO OK=NAZAD", CENTER_X, 116, TFT_GREEN);
  else if (languageMode == LANG_UA)
    drawCyrTextCentered("KOROTKO OK=NAZAD", CENTER_X, 116, TFT_GREEN);
  else
    tft.drawCentreString("Short OK=Back", CENTER_X, 115, 1);
}

static void drawCalibrationWizardStep(bool nanoTester, uint8_t step)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  if (nanoTester)
    tft.drawCentreString("NANO TESTER", CENTER_X, 21, 2);
  else
    drawVoltmeterTitle();

  tft.fillRoundRect(8, 39, 144, 58, 7, 0x0186);
  tft.drawRoundRect(8, 39, 144, 58, 7, TFT_CYAN);

  if (nanoTester)
  {
    if (step == 0)
    {
      if (languageMode == LANG_RU)
      {
        drawCyrTextCentered("SOEDINITE", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("WUPY 1-2-3", CENTER_X, 62, TFT_YELLOW);
      }
      else if (languageMode == LANG_UA)
      {
        drawCyrTextCentered("ZJEDNAJTE", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("WUPY 1-2-3", CENTER_X, 62, TFT_YELLOW);
      }
      else
      {
        tft.drawCentreString("SHORT PROBES", CENTER_X, 48, 1);
        tft.drawCentreString("1-2-3", CENTER_X, 63, 2);
      }
    }
    else if (step == 1)
    {
      if (languageMode == LANG_RU)
      {
        drawCyrTextCentered("RAZOMKNITE", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("VSE WUPY", CENTER_X, 62, TFT_YELLOW);
      }
      else if (languageMode == LANG_UA)
      {
        drawCyrTextCentered("ROZIMKNITb", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("USI WUPY", CENTER_X, 62, TFT_YELLOW);
      }
      else
      {
        tft.drawCentreString("OPEN ALL", CENTER_X, 48, 1);
        tft.drawCentreString("PROBES", CENTER_X, 63, 2);
      }
    }
    else
    {
      if (languageMode == LANG_RU)
      {
        drawCyrTextCentered("TEST PROJDEN", CENTER_X, 47, TFT_GREEN);
        drawCyrTextCentered("DANNYE NE ZAPIS.", CENTER_X, 64, TFT_YELLOW);
      }
      else if (languageMode == LANG_UA)
      {
        drawCyrTextCentered("TEST PROJDENO", CENTER_X, 47, TFT_GREEN);
        drawCyrTextCentered("DANI NE ZAPIS.", CENTER_X, 64, TFT_YELLOW);
      }
      else
      {
        tft.drawCentreString("TEST COMPLETE", CENTER_X, 48, 1);
        tft.drawCentreString("NO DATA WRITTEN", CENTER_X, 64, 1);
      }
    }
  }
  else
  {
    if (step == 0)
    {
      if (languageMode == LANG_RU)
      {
        drawCyrTextCentered("OTKL. NAPR.", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("NA VHODE 0V", CENTER_X, 62, TFT_YELLOW);
      }
      else if (languageMode == LANG_UA)
      {
        drawCyrTextCentered("VIDKL. NAPR.", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("NA VHODI 0V", CENTER_X, 62, TFT_YELLOW);
      }
      else
      {
        tft.drawCentreString("INPUT MUST BE", CENTER_X, 48, 1);
        tft.drawCentreString("0.000 V", CENTER_X, 63, 2);
      }
    }
    else if (step == 1)
    {
      if (languageMode == LANG_RU)
      {
        drawCyrTextCentered("PODAJTE ETALON", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("NAPRIMER 5.000V", CENTER_X, 62, TFT_YELLOW);
      }
      else if (languageMode == LANG_UA)
      {
        drawCyrTextCentered("PODAJTE ETALON", CENTER_X, 47, TFT_WHITE);
        drawCyrTextCentered("NAPRIKLAD 5.000V", CENTER_X, 62, TFT_YELLOW);
      }
      else
      {
        tft.drawCentreString("APPLY REFERENCE", CENTER_X, 48, 1);
        tft.drawCentreString("EXAMPLE 5.000V", CENTER_X, 63, 1);
      }
    }
    else
    {
      if (languageMode == LANG_RU)
      {
        drawCyrTextCentered("TEST PROJDEN", CENTER_X, 47, TFT_GREEN);
        drawCyrTextCentered("DANNYE NE ZAPIS.", CENTER_X, 64, TFT_YELLOW);
      }
      else if (languageMode == LANG_UA)
      {
        drawCyrTextCentered("TEST PROJDENO", CENTER_X, 47, TFT_GREEN);
        drawCyrTextCentered("DANI NE ZAPIS.", CENTER_X, 64, TFT_YELLOW);
      }
      else
      {
        tft.drawCentreString("TEST COMPLETE", CENTER_X, 48, 1);
        tft.drawCentreString("NO DATA WRITTEN", CENTER_X, 64, 1);
      }
    }
  }

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  if (step < 2)
    drawLocalizedOKSelectHint(CENTER_X, 116, TFT_GREEN);
  else if (languageMode == LANG_RU)
    drawCyrTextCentered("OK=NAZAD", CENTER_X, 116, TFT_GREEN);
  else if (languageMode == LANG_UA)
    drawCyrTextCentered("OK=NAZAD", CENTER_X, 116, TFT_GREEN);
  else
    tft.drawCentreString("OK=Back", CENTER_X, 115, 1);
}

static void calibrationWizardLoop(bool nanoTester)
{
  uint8_t step = 0;
  drawCalibrationWizardStep(nanoTester, step);

  while (true)
  {
    if (autoOffTick()) drawCalibrationWizardStep(nanoTester, step);

    if (buttonOK())
    {
      if (step < 2)
      {
        step++;
        soundClick();
        drawCalibrationWizardStep(nanoTester, step);
      }
      else
      {
        return;
      }
    }

    delay(5);
  }
}


static void updateVoltmeterCalibrationValues(float referenceVoltage,
                                              float measuredVoltage)
{
  // Обновляем только области с цифрами, без полной перерисовки экрана.
  tft.fillRect(54, 43, 90, 18, 0x0186);
  tft.fillRect(54, 62, 90, 18, 0x0186);

  tft.setTextColor(TFT_GREENYELLOW, 0x0186);
  tft.drawRightString(measuredVoltage < 0.0f ? "ERROR" :
                      String(measuredVoltage, 3) + "V", 143, 45, 2);

  tft.setTextColor(TFT_YELLOW, 0x0186);
  tft.drawRightString(String(referenceVoltage, 3) + "V", 143, 64, 2);
}

static void updateVoltmeterCalibrationProgress(bool holding, uint16_t heldMs)
{
  // Очищаем только внутреннюю часть полосы удержания.
  tft.fillRect(23, 95, 114, 3, 0x0186);

  if (!holding)
    return;

  int fill = map((int)heldMs, 0, 1800, 0, 112);
  if (fill < 0) fill = 0;
  if (fill > 112) fill = 112;

  if (fill > 0)
    tft.fillRect(24, 96, fill, 1, TFT_GREEN);
}

static void drawVoltmeterCalibrationScreen(float referenceVoltage, float measuredVoltage,
                                           bool holding, uint16_t heldMs)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  drawVoltmeterTitle();

  tft.fillRoundRect(8, 38, 144, 62, 7, 0x0186);
  tft.drawRoundRect(8, 38, 144, 62, 7, TFT_CYAN);

  tft.setTextColor(TFT_WHITE, 0x0186);
  tft.drawString("ADS:", 16, 46, 1);
  tft.drawString("REF:", 16, 65, 1);

  tft.setTextColor(TFT_CYAN, 0x0186);
  if (languageMode == LANG_RU)
    drawCyrTextCentered("VVERH/VNIZ ETALON", CENTER_X, 83, TFT_CYAN);
  else if (languageMode == LANG_UA)
    drawCyrTextCentered("VVERH/VNIZ ETALON", CENTER_X, 83, TFT_CYAN);
  else
    tft.drawCentreString("UP/DOWN REFERENCE", CENTER_X, 82, 1);

  tft.drawRect(22, 94, 116, 5, TFT_WHITE);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  if (languageMode == LANG_RU)
    drawCyrTextCentered("UDERJ.OK=SOHR.", CENTER_X, 116, TFT_GREEN);
  else if (languageMode == LANG_UA)
    drawCyrTextCentered("UTRIM.OK=ZBER.", CENTER_X, 116, TFT_GREEN);
  else
    tft.drawCentreString("Hold OK=Save", CENTER_X, 115, 1);

  updateVoltmeterCalibrationValues(referenceVoltage, measuredVoltage);
  updateVoltmeterCalibrationProgress(holding, heldMs);
}

static void showVoltmeterCalibrationSaved(float coefficient)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);
  drawVoltmeterTitle();

  tft.fillRoundRect(12, 43, 136, 48, 7, 0x0186);
  tft.drawRoundRect(12, 43, 136, 48, 7, TFT_GREEN);

  if (languageMode == LANG_RU)
    drawCyrTextCentered("SOHRANENO", CENTER_X, 50, TFT_GREEN);
  else if (languageMode == LANG_UA)
    drawCyrTextCentered("ZBEREJENO", CENTER_X, 50, TFT_GREEN);
  else
    tft.drawCentreString("SAVED", CENTER_X, 49, 2);

  tft.setTextColor(TFT_WHITE, 0x0186);
  tft.drawCentreString("K=" + String(coefficient, 5), CENTER_X, 70, 2);
  delay(1300);
}

static void voltmeterCalibrationLoop()
{
  float referenceVoltage = 5.000f;
  float measuredVoltage = readVoltmeterValue();
  drawVoltmeterCalibrationScreen(referenceVoltage, measuredVoltage, false, 0);
  unsigned long refreshTimer = 0;

  while (true)
  {
    if (autoOffTick())
      updateVoltmeterCalibrationValues(referenceVoltage, measuredVoltage);

    if (millis() - refreshTimer >= 250UL)
    {
      refreshTimer = millis();
      measuredVoltage = readVoltmeterValue();
      updateVoltmeterCalibrationValues(referenceVoltage, measuredVoltage);
    }

    if (buttonUp())
    {
      referenceVoltage += 0.010f;
      if (referenceVoltage > 30.000f) referenceVoltage = 30.000f;
      updateVoltmeterCalibrationValues(referenceVoltage, measuredVoltage);
    }

    if (buttonDown())
    {
      referenceVoltage -= 0.010f;
      if (referenceVoltage < 0.100f) referenceVoltage = 0.100f;
      updateVoltmeterCalibrationValues(referenceVoltage, measuredVoltage);
    }

    if (digitalRead(BTN_OK) == HIGH)
    {
      delay(35);
      if (digitalRead(BTN_OK) != HIGH) continue;

      // OK в калибровке также сразу сбрасывает таймер сна.
      userActivity();
      unsigned long started = millis();
      while (digitalRead(BTN_OK) == HIGH)
      {
        uint16_t held = (uint16_t)(millis() - started);
        updateVoltmeterCalibrationProgress(true, held);

        if (held >= 1800U)
        {
          while (digitalRead(BTN_OK) == HIGH) delay(5);

          if (measuredVoltage > 0.100f)
          {
            float newCoefficient = voltmeterCalibration * referenceVoltage / measuredVoltage;
            if (newCoefficient < 0.80f) newCoefficient = 0.80f;
            if (newCoefficient > 1.20f) newCoefficient = 1.20f;
            voltmeterCalibration = newCoefficient;
            saveUserSettings();
            soundFound();
            showVoltmeterCalibrationSaved(voltmeterCalibration);
          }
          else
          {
            soundError();
          }
          return;
        }
        delay(20);
      }

      soundClick();
      return;
    }

    delay(5);
  }
}

static void calibrationConfirmLoop(bool nanoTester)
{
  drawCalibrationConfirmScreen(nanoTester, false, 0);

  while (true)
  {
    if (autoOffTick()) drawCalibrationConfirmScreen(nanoTester, false, 0);

    if (digitalRead(BTN_OK) == HIGH)
    {
      delay(35);
      if (digitalRead(BTN_OK) != HIGH) continue;

      // OK в калибровке также сразу сбрасывает таймер сна.
      userActivity();
      unsigned long started = millis();
      while (digitalRead(BTN_OK) == HIGH)
      {
        unsigned long held = millis() - started;
        drawCalibrationConfirmScreen(nanoTester, true, held);

        if (held >= 1800UL)
        {
          soundLong();
          while (digitalRead(BTN_OK) == HIGH) delay(5);
          delay(80);
          calibrationWizardLoop(nanoTester);
          return;
        }
        delay(20);
      }

      // Короткое нажатие — назад
      soundClick();
      return;
    }

    delay(5);
  }
}

static void drawCalibrationStatusScreen()
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  drawCalibrationTitle();

  tft.fillRoundRect(10, 40, 140, 52, 7, 0x0186);
  tft.drawRoundRect(10, 40, 140, 52, 7, TFT_CYAN);

  if (languageMode == LANG_RU)
  {
    drawCyrText("NANO:", 18, 49, TFT_WHITE);
    drawCyrText("TEST BEZ ZAPISI", 56, 49, TFT_GREEN);
    drawCyrText("VOLbT:", 18, 69, TFT_WHITE);
    String calibrationText = "K=" + String(voltmeterCalibration, 4);
    drawCyrText(calibrationText.c_str(), 56, 69, TFT_GREEN);
  }
  else if (languageMode == LANG_UA)
  {
    drawCyrText("NANO:", 18, 49, TFT_WHITE);
    drawCyrText("TEST BEZ ZAPISU", 56, 49, TFT_GREEN);
    drawCyrText("VOLbT:", 18, 69, TFT_WHITE);
    String calibrationText = "K=" + String(voltmeterCalibration, 4);
    drawCyrText(calibrationText.c_str(), 56, 69, TFT_GREEN);
  }
  else
  {
    tft.setTextColor(TFT_WHITE, 0x0186);
    tft.drawString("NANO:", 18, 48, 1);
    tft.drawString("TEST NO WRITE", 56, 48, 1);
    tft.drawString("VOLT:", 18, 68, 1);
    tft.drawString("K=" + String(voltmeterCalibration, 4), 56, 68, 1);
  }

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSelectHint(CENTER_X, 116, TFT_GREEN);
}

static void calibrationStatusLoop()
{
  drawCalibrationStatusScreen();
  while (true)
  {
    if (autoOffTick()) drawCalibrationStatusScreen();
    if (buttonOK() || buttonOKLong(1200)) return;
    delay(5);
  }
}

void calibrationLoop()
{
  uint8_t selected = 0;
  drawCalibrationMenu(selected);

  while (true)
  {
    if (autoOffTick()) drawCalibrationMenu(selected);

    if (buttonUp())
    {
      selected = (selected == 0) ? CALIBRATION_COUNT - 1 : selected - 1;
      drawCalibrationMenu(selected);
    }

    if (buttonDown())
    {
      selected++;
      if (selected >= CALIBRATION_COUNT) selected = 0;
      drawCalibrationMenu(selected);
    }

    if (buttonOK())
    {
      if (selected == CALIBRATION_BACK_INDEX)
      {
        drawSettings(true);
        return;
      }
      else if (selected == CALIBRATION_NANO_INDEX)
        calibrationConfirmLoop(true);
      else if (selected == CALIBRATION_VOLTMETER_INDEX)
        voltmeterCalibrationLoop();
      else
        calibrationStatusLoop();

      drawCalibrationMenu(selected);
    }

    delay(5);
  }
}

//======================================================
// LANGUAGE SUBMENU
//======================================================

const uint8_t LANGUAGE_COUNT = 4;
const uint8_t LANGUAGE_EXIT_INDEX = 3;

void drawLanguageItem(uint8_t index, bool selected)
{
  int y = 38 + index * 18;
  tft.fillRect(8, y - 2, 144, 20, COLOR_BG);

  if (selected)
  {
    tft.fillRect(8, y - 1, 144, 18, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString(">", 18, y, 2);
    drawLocalizedLanguageItem(index, 34, y, (index == LANGUAGE_EXIT_INDEX) ? TFT_RED : TFT_WHITE);
  }
  else
  {
    uint16_t textColor = (index == LANGUAGE_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
    tft.setTextColor(textColor, COLOR_BG);
    drawLocalizedLanguageItem(index, 34, y, textColor);
  }

  bool active = false;
  if (index == 0 && languageMode == LANG_EN) active = true;
  if (index == 1 && languageMode == LANG_RU) active = true;
  if (index == 2 && languageMode == LANG_UA) active = true;

  if (active)
  {
    tft.setTextColor(TFT_GREEN, selected ? TFT_BLUE : COLOR_BG);
    tft.drawString("*", 132, y, 2);
  }
}

void drawLanguageMenu(uint8_t selected)
{
  clearScreen();
  drawStatusBar();
  drawFrame();
  drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);

  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  drawLocalizedLanguageTitle(CENTER_X, 22, TFT_YELLOW);

  for (uint8_t i = 0; i < LANGUAGE_COUNT; i++)
    drawLanguageItem(i, selected == i);

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  drawLocalizedOKSave(CENTER_X, 116, TFT_GREEN);
}

void languageLoop()
{
  uint8_t selected = (uint8_t)languageMode;
  if (selected > 2) selected = 0;
  uint8_t oldSelected = 255;
  drawLanguageMenu(selected);

  while (true)
  {
    if (autoOffTick()) drawLanguageMenu(selected);

    if (buttonUp()) selected = (selected == 0) ? LANGUAGE_COUNT - 1 : selected - 1;
    if (buttonDown()) { selected++; if (selected >= LANGUAGE_COUNT) selected = 0; }

    if (oldSelected != selected)
    {
      drawLanguageMenu(selected);
      oldSelected = selected;
    }

    if (buttonOK())
    {
      if (selected == LANGUAGE_EXIT_INDEX)
      {
        drawSettings(true);
        return;
      }

      saveLanguageMode((LanguageMode)selected);
      soundSaved();
      tft.fillRect(1, 91, 158, 20, COLOR_BG);
      tft.setTextColor(TFT_GREEN, COLOR_BG);
      drawLocalizedSavedShort(CENTER_X, 92, TFT_GREEN);
      delay(600);
      userActivity();
      drawLanguageMenu(selected);
    }

    delay(5);
  }
}


//======================================================
// SPLASH SUBMENU
//======================================================
const uint8_t SPLASH_COUNT = 6;
const uint8_t SPLASH_EXIT_INDEX = 5;
const uint8_t SPLASH_VISIBLE = 4;
uint8_t splashTop = 0;

void keepSplashVisible(uint8_t selected)
{
  if (selected < splashTop) splashTop = selected;
  if (selected >= splashTop + SPLASH_VISIBLE) splashTop = selected - SPLASH_VISIBLE + 1;
  if (splashTop + SPLASH_VISIBLE > SPLASH_COUNT) splashTop = SPLASH_COUNT - SPLASH_VISIBLE;
}

void drawSplashChoiceItem(uint8_t line, uint8_t index, bool selected)
{
  int y = 38 + line * 16;
  tft.fillRect(8, y - 2, 144, 17, COLOR_BG);
  uint16_t tc = (index == SPLASH_EXIT_INDEX) ? TFT_RED : TFT_WHITE;
  if (selected) {
    tft.fillRect(8, y - 1, 144, 16, TFT_BLUE);
    tft.setTextColor(tc, TFT_BLUE);
    tft.drawString(">", 18, y, 2);
  } else tft.setTextColor(tc, COLOR_BG);

  if (index < 5) {
    String number = String(index + 1);

    if (languageMode == LANG_RU || languageMode == LANG_UA)
    {
      // RU: ЗАСТАВКА 1...5
      // UA: ЗАСТАВКА 1...5
      drawCyrText("ZASTAVKA", 34, y + 3, tc);
      tft.setTextColor(tc, selected ? TFT_BLUE : COLOR_BG);
      tft.drawString(number, 112, y, 2);
    }
    else
    {
      String name = String("Splash ") + number;
      tft.drawString(name, 34, y, 2);
    }

    if (index == bootStyle) {
      tft.setTextColor(TFT_GREEN, selected ? TFT_BLUE : COLOR_BG);
      tft.drawString("*", 136, y, 2);
    }
  } else {
    if (languageMode == LANG_UA) drawCyrText("VIXiD", 34, y + 3, tc);
    else if (languageMode == LANG_RU) drawCyrText("VYXOD", 34, y + 3, tc);
    else tft.drawString("Exit", 34, y, 2);
  }
}

void drawSplashChoiceMenu(uint8_t selected)
{
  keepSplashVisible(selected);
  clearScreen(); drawStatusBar(); drawFrame(); drawBottomBar();
  tft.fillRect(1, 17, 158, 94, COLOR_BG);
  tft.setTextColor(TFT_YELLOW, COLOR_BG);
  if (languageMode == LANG_UA || languageMode == LANG_RU)
    drawCyrTextCentered("ZASTAVKA", CENTER_X, 25, TFT_YELLOW);
  else
    tft.drawCentreString("SPLASH SCREEN", CENTER_X, 22, 2);
  for (uint8_t line=0; line<SPLASH_VISIBLE; line++) {
    uint8_t item=splashTop+line;
    drawSplashChoiceItem(line,item,item==selected);
  }
  tft.fillRect(150, 36, 8, 72, COLOR_BG);
  tft.setTextColor(TFT_CYAN, COLOR_BG);
  if (splashTop>0) tft.drawString("^",151,38,1);
  if (splashTop+SPLASH_VISIBLE<SPLASH_COUNT) tft.drawString("v",151,100,1);
  tft.setTextColor(TFT_GREEN,TFT_DARKGREY);
  drawLocalizedOKSelectHint(CENTER_X,116,TFT_GREEN);
}

void splashChoiceLoop()
{
  uint8_t selected=bootStyle;
  uint8_t oldSelected=255;
  splashTop=0;
  drawSplashChoiceMenu(selected);
  while(true) {
    if (autoOffTick()) drawSplashChoiceMenu(selected);
    if (buttonUp()) selected=(selected==0)?SPLASH_COUNT-1:selected-1;
    if (buttonDown()) { selected++; if(selected>=SPLASH_COUNT) selected=0; }
    if (selected!=oldSelected) { drawSplashChoiceMenu(selected); oldSelected=selected; }
    if (buttonOK()) {
      if (selected==SPLASH_EXIT_INDEX) { drawSettings(true); return; }
      bootStyle=clampBootStyle(selected);
      saveUserSettings(); soundSaved();
      tft.fillRect(1,91,158,20,COLOR_BG);
      drawLocalizedSavedShort(CENTER_X,92,TFT_GREEN);
      delay(500); userActivity(); drawSplashChoiceMenu(selected);
    }
    delay(5);
  }
}

//======================================================
// settingsLoop()
//======================================================

void settingsLoop()
{
  settingsIndex = 0;
  settingsTopIndex = 0;
  oldSettingsIndex = 255;
  oldSettingsTopIndex = 255;
  drawSettings(true);

  while (true)
  {
    if (autoOffTick()) drawSettings(true);

    if (buttonUp())
    {
      settingsIndex = (settingsIndex == 0) ? SETTINGS_COUNT - 1 : settingsIndex - 1;
      drawSettings(false);
    }

    if (buttonDown())
    {
      settingsIndex++;
      if (settingsIndex >= SETTINGS_COUNT) settingsIndex = 0;
      drawSettings(false);
    }

    if (buttonOK())
    {
      if (settingsIndex == SETTINGS_EXIT_INDEX)
      {
        drawMenu(true);
        return;
      }

      if (settingsIndex == SETTINGS_DISPLAY_INDEX)
        displayBrightnessLoop();
      else if (settingsIndex == SETTINGS_SOUND_INDEX)
        soundVolumeLoop();
      else if (settingsIndex == SETTINGS_AUTO_OFF_INDEX)
        autoOffLoop();
      else if (settingsIndex == SETTINGS_CALIBRATION_INDEX)
        calibrationLoop();
      else if (settingsIndex == SETTINGS_STARTUP_INDEX)
        startupModeLoop();
      else if (settingsIndex == SETTINGS_SPLASH_INDEX)
        splashChoiceLoop();
      else if (settingsIndex == SETTINGS_LANGUAGE_INDEX)
        languageLoop();
      else
      {
        tft.fillRect(1, 88, 158, 23, COLOR_BG);
        tft.setTextColor(TFT_GREEN, COLOR_BG);
        tft.drawCentreString(txtComingSoon(), CENTER_X, 92, 2);
        delay(500);
        userActivity();
        drawSettings(true);
      }
    }

    delay(5);
  }
}
