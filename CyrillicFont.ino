/****************************************************************
  CyrillicFont.ino

  Маленький векторный кириллический шрифт 7px.
  Основа — шрифт из старого скетча пользователя.

  Пока файл добавлен как заготовка для нормального RU/UA меню.
  На этом этапе основные drawString() ещё не заменены массово,
  чтобы не сломать интерфейс. Следующий шаг — функции слов меню.
****************************************************************/

// --- БУКВЫ ---
void b_A(int x, int y, uint16_t c) { tft.drawLine(x, y+7, x+3, y, c); tft.drawLine(x+3, y, x+6, y+7, c); tft.drawFastHLine(x+2, y+4, 3, c); }
void b_K(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawLine(x, y+3, x+5, y, c); tft.drawLine(x, y+3, x+5, y+7, c); }
void b_L(int x, int y, uint16_t c) { tft.drawLine(x, y+7, x+2, y, c); tft.drawFastHLine(x+2, y, 3, c); tft.drawFastVLine(x+5, y, 7, c); }
void b_U(int x, int y, uint16_t c) { tft.drawLine(x, y, x+2, y+3, c); tft.drawLine(x+4, y, x, y+7, c); }
void b_M(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+6, y, 7, c); tft.drawLine(x, y, x+3, y+3, c); tft.drawLine(x+6, y, x+3, y+3, c); }
void b_R(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x, y+3, 5, c); tft.drawFastVLine(x+5, y, 4, c); }
void b_Z(int x, int y, uint16_t c) { tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x, y+3, 4, c); tft.drawFastHLine(x, y+6, 5, c); tft.drawFastVLine(x+5, y, 4, c); tft.drawFastVLine(x+5, y+3, 4, c); }
void b_YA(int x, int y, uint16_t c) {
  // Буква Я: сделана с нормальным верхним левым углом, без среза.
  tft.drawFastHLine(x+1, y, 5, c);
  tft.drawFastVLine(x+5, y, 7, c);
  tft.drawFastHLine(x+1, y+3, 5, c);
  tft.drawFastVLine(x+1, y, 4, c);
  tft.drawLine(x+3, y+3, x, y+7, c);
}
void b_O(int x, int y, uint16_t c) { tft.drawRect(x, y, 6, 7, c); }
void b_J(int x, int y, uint16_t c) { tft.drawFastVLine(x+3, y, 7, c); tft.drawLine(x, y, x+6, y+7, c); tft.drawLine(x+6, y, x, y+7, c); }
void b_I(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawLine(x+5, y, x, y+7, c); }
void b_D(int x, int y, uint16_t c) { tft.drawFastHLine(x, y, 6, c); tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawFastHLine(x-1, y+6, 8, c); }
void b_E(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x, y+3, 4, c); tft.drawFastHLine(x, y+6, 5, c); }
void b_H(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawFastHLine(x, y+3, 5, c); }
// Н отдельным именем, чтобы не путаться в словах
void b_N(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawFastHLine(x, y+3, 6, c); }
void b_T(int x, int y, uint16_t c) { tft.drawFastHLine(x, y, 7, c); tft.drawFastVLine(x+3, y, 7, c); }
void b_P(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawFastHLine(x, y, 6, c); }
void b_V(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x, y+3, 5, c); tft.drawFastHLine(x, y+6, 5, c); tft.drawFastVLine(x+5, y, 4, c); tft.drawFastVLine(x+5, y+3, 4, c); }
void b_C(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x, y+6, 5, c); }
void b_SH(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+3, y, 7, c); tft.drawFastVLine(x+6, y, 7, c); tft.drawFastHLine(x, y+6, 7, c); }
void b_SOFT(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y+3, 4, c); tft.drawFastHLine(x, y+6, 4, c); tft.drawFastVLine(x+4, y+3, 4, c); }

// --- УКРАИНСКИЕ ДОБАВКИ, ПЕРВАЯ ЗАГОТОВКА ---
// І
void b_UA_I(int x, int y, uint16_t c) { tft.drawFastHLine(x, y, 5, c); tft.drawFastVLine(x+2, y, 7, c); tft.drawFastHLine(x, y+6, 5, c); }
// Ї
void b_UA_YI(int x, int y, uint16_t c) { tft.fillRect(x+1, y-2, 1, 1, c); tft.fillRect(x+4, y-2, 1, 1, c); b_UA_I(x, y, c); }
// Є
void b_UA_YE(int x, int y, uint16_t c) { tft.drawFastVLine(x+5, y, 7, c); tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x+1, y+3, 4, c); tft.drawFastHLine(x, y+6, 5, c); }
// Ґ
void b_UA_GHE(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 6, c); tft.drawFastHLine(x+4, y-2, 3, c); }

// --- СТАРЫЕ ГОТОВЫЕ СЛОВА ---
void word_PULT(int x, int y, uint16_t c) { b_P(x, y, c); b_U(x + 7, y, c); b_L(x + 14, y, c); b_SOFT(x + 21, y, c); b_T(x + 28, y, c); }
void word_REJIM(int x, int y, uint16_t c) { b_R(x,y,c); b_E(x+7,y,c); b_J(x+14,y,c); b_I(x+23,y,c); b_M(x+31,y,c); tft.fillRect(x+39, y+1, 2, 2, c); tft.fillRect(x+39, y+5, 2, 2, c); }
void word_BJT(int x, int y, uint16_t c) { b_T(x,y,c); b_R(x+8,y,c); b_A(x+15,y,c); b_H(x+23,y,c); b_Z(x+30,y,c); b_I(x+37,y,c); b_C(x+45,y,c); b_T(x+52,y,c); b_O(x+60,y,c); b_R(x+68,y,c); }
void word_DIOD(int x, int y, uint16_t c) { b_D(x,y,c); b_I(x+7,y,c); b_O(x+15,y,c); b_D(x+23,y,c); }
void word_ZENER(int x, int y, uint16_t c) { b_Z(x,y,c); b_E(x+7,y,c); b_H(x+14,y,c); b_E(x+21,y,c); b_R(x+28,y,c); }
void word_RES(int x, int y, uint16_t c) { b_R(x,y,c); b_E(x+7,y,c); b_Z(x+14,y,c); b_I(x+21,y,c); b_C(x+29,y,c); b_T(x+36,y,c); b_O(x+44,y,c); b_R(x+52,y,c); }
void word_CAP(int x, int y, uint16_t c) { b_E(x,y,c); b_M(x+8,y,c); b_K(x+16,y,c); b_O(x+23,y,c); b_C(x+31,y,c); b_T(x+38,y,c); b_SOFT(x+46,y,c); }
void word_IND(int x, int y, uint16_t c) { b_K(x,y,c); b_A(x+7,y,c); b_T(x+15,y,c); b_U(x+22,y,c); b_SH(x+30,y,c); b_K(x+38,y,c); b_A(x+45,y,c); }
void word_SOPROT(int x, int y, uint16_t c) { b_C(x,y,c); b_O(x+7,y,c); b_P(x+15,y,c); b_R(x+22,y,c); b_O(x+29,y,c); tft.drawFastVLine(x+36,y,7,c); tft.drawFastHLine(x+34,y,5,c); b_I(x+43,y,c); b_V(x+50,y,c); b_L(x+58,y,c); b_E(x+65,y,c); b_H(x+72,y,c); b_I(x+79,y,c); b_E(x+86,y,c); }
void word_PRIVET(int x, int y, uint16_t c) { b_P(x, y, c); b_R(x+8, y, c); b_I(x+15, y, c); b_V(x+22, y, c); b_E(x+29, y, c); b_T(x+36, y, c); }
void word_AKKUM_RAZR(int x, int y, uint16_t c) { b_A(x, y, c); b_K(x+7, y, c); b_K(x+14, y, c); b_U(x+21, y, c); b_M(x+28, y, c); b_U(x+36, y, c); b_L(x+43, y, c); b_YA(x+50, y, c); b_T(x+57, y, c); b_O(x+65, y, c); b_R(x+72, y, c); }


//======================================================
// Дополнительные буквы для меню RU/UA
//======================================================

// Б
void b_B(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 6, c); tft.drawFastHLine(x, y+3, 5, c); tft.drawFastHLine(x, y+6, 5, c); tft.drawFastVLine(x+5, y+3, 4, c); }
// Ф
void b_F(int x, int y, uint16_t c) { tft.drawFastVLine(x+3, y, 7, c); tft.drawRect(x, y+1, 3, 5, c); tft.drawRect(x+4, y+1, 3, 5, c); }
// Х
void b_X(int x, int y, uint16_t c) { tft.drawLine(x, y, x+6, y+7, c); tft.drawLine(x+6, y, x, y+7, c); }
// Ы
void b_Y(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y+3, 4, c); tft.drawFastHLine(x, y+6, 4, c); tft.drawFastVLine(x+4, y+3, 4, c); tft.drawFastVLine(x+6, y, 7, c); }
// Э
void b_EH(int x, int y, uint16_t c) { tft.drawFastHLine(x, y, 5, c); tft.drawFastHLine(x+1, y+3, 4, c); tft.drawFastHLine(x, y+6, 5, c); tft.drawFastVLine(x+5, y, 7, c); }
// Й
void b_SHORTI(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawLine(x+5, y, x, y+7, c); tft.drawFastHLine(x+2, y-2, 3, c); }
// Ч
void b_CH(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 4, c); tft.drawFastVLine(x+5, y, 7, c); tft.drawFastHLine(x, y+3, 5, c); }

// Г
void b_G(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y, 6, c); }
// Ю
void b_YU(int x, int y, uint16_t c) { tft.drawFastVLine(x, y, 7, c); tft.drawFastHLine(x, y+3, 3, c); tft.drawRect(x+3, y, 5, 7, c); }

//======================================================
// Мини-печать кириллицей через служебные ASCII-коды
// A=А B=Б C=С D=Д E=Е e=Э F=Ф G=Г N=Н H=Н I=И j=Й i=І
// J=Ж K=К L=Л M=М O=О P=П Q=Я R=Р S=С T=Т
// U=У V=В W=Ш X=Х Y=Ы Z=З b=Ь u=Ю Ч=ч через h
//======================================================

uint8_t cyrCharWidth(char ch)
{
  if (ch == ' ') return 4;
  if (ch == '.') return 3;
  if (ch == '-') return 5;
  if (ch == '=') return 6;
  if (ch == 'I' || ch == 'i') return 6;
  if (ch == 'u') return 9;
  return 7;
}

void drawCyrChar(char ch, int x, int y, uint16_t c)
{
  switch (ch)
  {
    case 'A': b_A(x,y,c); break;
    case 'B': b_B(x,y,c); break;
    case 'C': b_C(x,y,c); break;
    case 'D': b_D(x,y,c); break;
    case 'E': b_E(x,y,c); break;
    case 'e': b_EH(x,y,c); break;
    case 'F': b_F(x,y,c); break;
    case 'G': b_G(x,y,c); break;
    case 'H': b_H(x,y,c); break;
    case 'I': b_I(x,y,c); break;
    case 'i': b_UA_I(x,y,c); break;
    case 'j': b_SHORTI(x,y,c); break;
    case 'J': b_J(x,y,c); break;
    case 'K': b_K(x,y,c); break;
    case 'L': b_L(x,y,c); break;
    case 'M': b_M(x,y,c); break;
    case 'N': b_N(x,y,c); break;
    case 'O': b_O(x,y,c); break;
    case 'P': b_P(x,y,c); break;
    case 'Q': b_YA(x,y,c); break;
    case 'R': b_R(x,y,c); break;
    case 'S': b_C(x,y,c); break;
    case 'T': b_T(x,y,c); break;
    case 'U': b_U(x,y,c); break;
    case 'u': b_YU(x,y,c); break;
    case 'V': b_V(x,y,c); break;
    case 'W': b_SH(x,y,c); break;
    case 'X': b_X(x,y,c); break;
    case 'Y': b_Y(x,y,c); break;
    case 'Z': b_Z(x,y,c); break;
    case 'b': b_SOFT(x,y,c); break;
    case 'h': b_CH(x,y,c); break;
    case '-': tft.drawFastHLine(x, y+3, 5, c); break;
    case '=': tft.drawFastHLine(x, y+2, 5, c); tft.drawFastHLine(x, y+5, 5, c); break;
    case '.': tft.fillRect(x, y+6, 2, 2, c); break;
    default: break;
  }
}

int cyrTextWidth(const char *s)
{
  int w = 0;
  while (*s)
  {
    w += cyrCharWidth(*s) + 1;
    s++;
  }
  if (w > 0) w--;
  return w;
}

void drawCyrText(const char *s, int x, int y, uint16_t c)
{
  while (*s)
  {
    drawCyrChar(*s, x, y, c);
    x += cyrCharWidth(*s) + 1;
    s++;
  }
}

void drawCyrTextCentered(const char *s, int cx, int y, uint16_t c)
{
  drawCyrText(s, cx - cyrTextWidth(s) / 2, y, c);
}

bool useCyrText()
{
  return languageMode == LANG_RU || languageMode == LANG_UA;
}

//======================================================
// Слова меню кириллицей. Строки записаны служебными кодами.
//======================================================

const char* cyrMainMenuTitle()
{
  if (languageMode == LANG_RU) return "GLAVNOE MENu";       // ГЛАВНОЕ МЕНЮ
  if (languageMode == LANG_UA) return "GOLOVNE MENu";
  return "MAIN MENU";
}

const char* cyrMenuItem(uint8_t index)
{
  if (languageMode == LANG_RU)
  {
    switch (index)
    {
      case 0: return "TESTER";       // ТЕСТЕР
      case 1: return "PROBNIK";      // ПРОБНИК
      case 2: return "VOLbTMETR";    // ВОЛЬТМЕТР
      case 3: return "PULbT";        // ПУЛЬТ
      case 4: return "NASTROjKI";    // НАСТРОЙКИ
      case 5: return "INFO";         // ИНФО
      default: return "VYXOD";       // ВЫХОД
    }
  }
  else if (languageMode == LANG_UA)
  {
    switch (index)
    {
      case 0: return "TESTER";       // ТЕСТЕР
      case 1: return "PROBNIK";      // ПРОБНИК
      case 2: return "VOLbTMETR";    // ВОЛЬТМЕТР
      case 3: return "PULbT";        // ПУЛЬТ
      case 4: return "NALAWTUV.";    // НАЛАШТУВ.
      case 5: return "INFO";         // ІНФО/ИНФО
      default: return "VIXiD";       // ВИХІД
    }
  }
  return "";
}

const char* cyrSettingsTitle()
{
  if (languageMode == LANG_RU) return "NASTROjKI";
  if (languageMode == LANG_UA) return "NALAWTUV.";
  return "SETTINGS";
}

const char* cyrSettingsItem(uint8_t index)
{
  if (languageMode == LANG_RU)
  {
    switch (index)
    {
      case 0: return "eKRAN";        // ЭКРАН
      case 1: return "ZVUK";         // ЗВУК
      case 2: return "AVTO OTKL.";  // АВТО ОТКЛ.
      case 3: return "KALIBROVKA";   // КАЛИБРОВКА
      case 4: return "ZAPUSK";       // ЗАПУСК
      case 5: return "ZASTAVKA";    // ЗАСТАВКА
      case 6: return "QZYK";         // ЯЗЫК
      default: return "VYXOD";       // ВЫХОД
    }
  }
  else if (languageMode == LANG_UA)
  {
    switch (index)
    {
      case 0: return "EKRAN";        // ЕКРАН
      case 1: return "ZVUK";         // ЗВУК
      case 2: return "AVTO VIMK.";  // АВТО ВИМК.
      case 3: return "KALiBRUV.";    // КАЛІБРУВ.
      case 4: return "ZAPUSK";       // ЗАПУСК
      case 5: return "ZASTAVKA";    // ЗАСТАВКА
      case 6: return "MOVA";         // МОВА
      default: return "VIXiD";       // ВИХІД
    }
  }
  return "";
}

const char* cyrNoComponentText()
{
  if (languageMode == LANG_RU) return "NET DETALI";  // НЕТ ДЕТАЛИ
  if (languageMode == LANG_UA) return "NEMA DETALi"; // НЕМА ДЕТАЛІ
  return "NO COMPONENT";
}

//======================================================
// Рисование локализованных пунктов/заголовков
//======================================================

void drawLocalizedMenuItem(uint8_t index, int x, int y, uint16_t c)
{
  if (useCyrText()) drawCyrText(cyrMenuItem(index), x, y + 3, (index == 7) ? TFT_RED : c);
  else { if (index == 7) tft.setTextColor(TFT_RED); tft.drawString(txtMenuItem(index), x, y, 2); }
}

void drawLocalizedSettingsItem(uint8_t index, int x, int y, uint16_t c)
{
  if (useCyrText()) drawCyrText(cyrSettingsItem(index), x, y + 3, (index == 7) ? TFT_RED : c);
  else { if (index == 7) tft.setTextColor(TFT_RED); tft.drawString(txtSettingsItem(index), x, y, 2); }
}

void drawLocalizedMainMenuTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrMainMenuTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtMainMenuTitle(), cx, y, 2);
}

void drawLocalizedSettingsTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrSettingsTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtSettingsTitle(), cx, y, 2);
}

void drawLocalizedNoComponent(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrNoComponentText(), cx, y + 2, c);
  else tft.drawCentreString(txtNoComponent(), cx, y, 2);
}

const char* cyrLanguageTitle()
{
  if (languageMode == LANG_RU) return "QZYK";   // ЯЗЫК
  if (languageMode == LANG_UA) return "MOVA";   // МОВА
  return "LANGUAGE";
}

const char* cyrLanguageExit()
{
  if (languageMode == LANG_RU) return "VYXOD";  // ВЫХОД
  if (languageMode == LANG_UA) return "VIXiD";  // ВИХІД
  return "Exit";
}

void drawLocalizedLanguageTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrLanguageTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtLanguageTitle(), cx, y, 2);
}

void drawLocalizedLanguageItem(uint8_t index, int x, int y, uint16_t c)
{
  if (useCyrText() && index == 3) drawCyrText(cyrLanguageExit(), x, y + 3, TFT_RED);
  else { if (index == 3) tft.setTextColor(TFT_RED); tft.drawString(txtLanguageItem(index), x, y, 2); }
}


//======================================================
// Дополнительные локализованные подписи для IR и нижней строки
//======================================================

const char* cyrOKSelectText()
{
  if (languageMode == LANG_RU) return "OK=VYBOR";   // ОК=ВЫБОР
  if (languageMode == LANG_UA) return "OK=VIBiR";   // ОК=ВИБІР
  return "OK=Select";
}

const char* cyrIRTitle()
{
  if (languageMode == LANG_RU) return "PULbT";      // ПУЛЬТ
  if (languageMode == LANG_UA) return "PULbT";      // ПУЛЬТ
  return "IR REMOTE";
}

const char* cyrIRPrompt()
{
  if (languageMode == LANG_RU) return "NAJMI KNOPKU";    // НАЖМИ КНОПКУ
  if (languageMode == LANG_UA) return "NATISNI KNOPKU";   // НАТИСНИ КНОПКУ
  return "Press remote key";
}

const char* cyrHoldOKMenu()
{
  if (languageMode == LANG_RU) return "UDERJ.   OK=MENu"; // УДЕРЖ.   ОК=МЕНЮ
  if (languageMode == LANG_UA) return "UTRIM.   OK=MENu"; // УТРИМ.   ОК=МЕНЮ
  return "Hold OK = Menu";
}

void drawLocalizedOKSelectHint(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrOKSelectText(), cx, y + 3, c);
  else tft.drawCentreString(txtOKSelect(), cx, y, 1);
}

void drawLocalizedIRTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrIRTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtIRTitle(), cx, y, 2);
}

void drawLocalizedIRPrompt(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrIRPrompt(), cx, y + 3, c);
  else tft.drawCentreString(txtIRPrompt(), cx, y, 2);
}

void drawLocalizedHoldOKMenu(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrHoldOKMenu(), cx, y + 3, c);
  else tft.drawCentreString(txtHoldOKMenu(), cx, y, 1);
}

void drawLocalizedTesterHint(int cx, int y, uint16_t c)
{
  if (languageMode == LANG_RU)
  {
    drawCyrTextCentered("OK=TEST UDERJ=MENu", cx, y + 2, c);  // ОК=ТЕСТ УДЕРЖ=МЕНЮ
    return;
  }

  if (languageMode == LANG_UA)
  {
    drawCyrTextCentered("OK=TEST UTRIM=MENu", cx, y + 2, c); // ОК=ТЕСТ УТРИМ=МЕНЮ
    return;
  }

  tft.drawCentreString("OK=Test Hold=Menu", cx, y, 1);
}

//======================================================
// Подменю ЗВУК: кириллические подписи RU/UA
//======================================================

const char* cyrSoundTitle()
{
  if (languageMode == LANG_RU) return "ZVUK";   // ЗВУК
  if (languageMode == LANG_UA) return "ZVUK";   // ЗВУК
  return "SOUND";
}

const char* cyrSoundExit()
{
  if (languageMode == LANG_RU) return "VYXOD";  // ВЫХОД
  if (languageMode == LANG_UA) return "VIXiD";  // ВИХІД
  return "Exit";
}

const char* cyrOKSave()
{
  if (languageMode == LANG_RU) return "OK=SOXR.";  // ОК=СОХР.
  if (languageMode == LANG_UA) return "OK=ZBER.";  // ОК=ЗБЕР.
  return "OK=Save";
}

const char* cyrSavedShort()
{
  if (languageMode == LANG_RU) return "SOXR.";      // СОХР.
  if (languageMode == LANG_UA) return "ZBER.";      // ЗБЕР.
  return "Saved";
}

void drawLocalizedSoundTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrSoundTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtSoundTitle(), cx, y, 2);
}

void drawLocalizedSoundItem(uint8_t itemIndex, int x, int y, uint16_t c)
{
  if (useCyrText())
  {
    if (itemIndex == 5) drawCyrText(cyrSoundExit(), x, y + 3, TFT_RED);
    else
    {
      switch (itemIndex)
      {
        case 0:
          if (languageMode == LANG_RU) drawCyrText("OTKL.", x, y + 3, c);
          else drawCyrText("VIMK.", x, y + 3, c);
          break;
        case 1: tft.drawString("25%", x, y, 2); break;
        case 2: tft.drawString("50%", x, y, 2); break;
        case 3: tft.drawString("75%", x, y, 2); break;
        case 4: tft.drawString("100%", x, y, 2); break;
      }
    }
  }
  else
  {
    switch (itemIndex)
    {
      case 0: tft.drawString("Off", x, y, 2); break;
      case 1: tft.drawString("25%", x, y, 2); break;
      case 2: tft.drawString("50%", x, y, 2); break;
      case 3: tft.drawString("75%", x, y, 2); break;
      case 4: tft.drawString("100%", x, y, 2); break;
      case 5: tft.setTextColor(TFT_RED); tft.drawString(txtExit(), x, y, 2); break;
    }
  }
}

void drawLocalizedOKSave(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrOKSave(), cx, y + 2, c);
  else tft.drawCentreString(txtOKSave(), cx, y, 1);
}

void drawLocalizedSavedShort(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrSavedShort(), cx, y + 3, c);
  else tft.drawCentreString(txtSaved(), cx, y, 2);
}



//======================================================
// Подменю ЭКРАН: яркость дисплея RU/UA
//======================================================

const char* cyrDisplayBrightnessTitle()
{
  if (languageMode == LANG_RU) return "QRKOSTb";   // ЯРКОСТЬ
  if (languageMode == LANG_UA) return "QRKiSTb";   // ЯРКІСТЬ
  return "BRIGHTNESS";
}

const char* cyrDisplayBrightnessExit()
{
  if (languageMode == LANG_RU) return "VYXOD";
  if (languageMode == LANG_UA) return "VIXiD";
  return "Exit";
}

void drawLocalizedDisplayBrightnessTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrDisplayBrightnessTitle(), cx, y + 3, c);
  else tft.drawCentreString("BRIGHTNESS", cx, y, 2);
}

void drawLocalizedDisplayBrightnessItem(uint8_t index, int x, int y, uint16_t c)
{
  if (index < 4)
  {
    const char* items[] = { "25%", "50%", "75%", "100%" };
    tft.setTextColor(c);
    tft.drawString(items[index], x, y, 2);
  }
  else
  {
    if (useCyrText()) drawCyrText(cyrDisplayBrightnessExit(), x, y + 3, TFT_RED);
    else
    {
      tft.setTextColor(TFT_RED);
      tft.drawString(txtExit(), x, y, 2);
    }
  }
}

//======================================================
// Подменю АВТО ОТКЛ: кириллические подписи RU/UA
//======================================================

const char* cyrAutoOffTitle()
{
  if (languageMode == LANG_RU) return "AVTO OTKL.";  // АВТО ОТКЛ.
  if (languageMode == LANG_UA) return "AVTO VIMK.";  // АВТО ВИМК.
  return "AUTO OFF";
}

const char* cyrAutoOffMinText()
{
  if (languageMode == LANG_RU) return "MIN";   // МИН
  if (languageMode == LANG_UA) return "XV";    // ХВ
  return "min";
}

const char* cyrAutoOffOffText()
{
  if (languageMode == LANG_RU) return "VYKL.";  // ВЫКЛ.
  if (languageMode == LANG_UA) return "VIMK.";  // ВИМК.
  return "Off";
}

const char* cyrAutoOffExitText()
{
  if (languageMode == LANG_RU) return "VYXOD";  // ВЫХОД
  if (languageMode == LANG_UA) return "VIXiD";  // ВИХІД
  return "Exit";
}

void drawLocalizedAutoOffTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrAutoOffTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtAutoOffTitle(), cx, y, 2);
}

void drawLocalizedAutoOffItem(uint8_t itemIndex, int x, int y, uint16_t c)
{
  uint16_t outColor = (itemIndex == 4) ? TFT_RED : c;

  if (useCyrText())
  {
    if (itemIndex <= 2)
    {
      const char *num = (itemIndex == 0) ? "1" : ((itemIndex == 1) ? "2" : "5");
      tft.setTextColor(outColor);
      tft.drawString(num, x, y, 2);
      drawCyrText(cyrAutoOffMinText(), x + 16, y + 3, outColor);
    }
    else if (itemIndex == 3) drawCyrText(cyrAutoOffOffText(), x, y + 3, outColor);
    else drawCyrText(cyrAutoOffExitText(), x, y + 3, TFT_RED);
  }
  else
  {
    const char *enItems[] = { "1 min", "2 min", "5 min", "Off", "Exit" };
    tft.setTextColor(outColor);
    tft.drawString(enItems[itemIndex], x, y, 2);
  }
}

//======================================================
// Подменю ЗАПУСК: кириллические подписи RU/UA
//======================================================

const char* cyrStartupTitle()
{
  if (languageMode == LANG_RU) return "ZAPUSK";  // ЗАПУСК
  if (languageMode == LANG_UA) return "ZAPUSK";  // ЗАПУСК
  return "STARTUP";
}

const char* cyrStartupItem(uint8_t index)
{
  if (languageMode == LANG_RU)
  {
    switch (index)
    {
      case 0: return "TESTER";       // ТЕСТЕР
      case 1: return "VOLbTMETR";    // ВОЛЬТМЕТР
      default: return "VYXOD";       // ВЫХОД
    }
  }
  if (languageMode == LANG_UA)
  {
    switch (index)
    {
      case 0: return "TESTER";       // ТЕСТЕР
      case 1: return "VOLbTMETR";    // ВОЛЬТМЕТР
      default: return "VIXiD";       // ВИХІД
    }
  }
  return txtStartupItem(index);
}

void drawLocalizedStartupTitle(int cx, int y, uint16_t c)
{
  if (useCyrText()) drawCyrTextCentered(cyrStartupTitle(), cx, y + 3, c);
  else tft.drawCentreString(txtStartupTitle(), cx, y, 2);
}

void drawLocalizedStartupItem(uint8_t index, int x, int y, uint16_t c)
{
  if (useCyrText()) drawCyrText(cyrStartupItem(index), x, y + 3, (index == 2) ? TFT_RED : c);
  else
  {
    tft.setTextColor((index == 2) ? TFT_RED : c);
    tft.drawString(txtStartupItem(index), x, y, 2);
  }
}
