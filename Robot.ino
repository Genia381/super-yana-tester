#include "Config.h"

//======================================================
// Робот
// x,y - левый верхний угол
//======================================================

void drawRobot(int x, int y, bool handUp)
{
    // Голова
    tft.fillRoundRect(x + 8, y, 32, 28, 4, TFT_LIGHTGREY);
    tft.drawRoundRect(x + 8, y, 32, 28, 4, TFT_WHITE);

    // Антенна
    tft.drawFastVLine(x + 24, y - 6, 6, TFT_WHITE);
    tft.fillCircle(x + 24, y - 8, 2, TFT_RED);

    // Глаза
    tft.fillCircle(x + 17, y + 10, 2, TFT_CYAN);
    tft.fillCircle(x + 31, y + 10, 2, TFT_CYAN);

    // Рот
    tft.drawFastHLine(x + 18, y + 19, 12, TFT_BLACK);

    // Тело
    tft.fillRoundRect(x + 12, y + 32, 24, 26, 3, TFT_LIGHTGREY);
    tft.drawRoundRect(x + 12, y + 32, 24, 26, 3, TFT_WHITE);

    // Левая рука
    tft.drawLine(x + 12, y + 36, x, y + 46, TFT_WHITE);

    // Правая рука
    if (handUp)
    {
        tft.drawLine(x + 36, y + 36, x + 48, y + 18, TFT_YELLOW);
    }
    else
    {
        tft.drawLine(x + 36, y + 36, x + 48, y + 46, TFT_YELLOW);
    }

    // Ноги
    tft.drawLine(x + 18, y + 58, x + 14, y + 72, TFT_WHITE);
    tft.drawLine(x + 30, y + 58, x + 34, y + 72, TFT_WHITE);
}

//======================================================
// Робот машет рукой
//======================================================

void robotWave(int x, int y)
{
    for (int i = 0; i < 6; i++)
    {
        drawMainScreen();

        drawRobot(x, y, true);

        delay(150);

        drawMainScreen();

        drawRobot(x, y, false);

        delay(150);
    }
}

//======================================================
// Красивый робот ожидания детали в режиме TESTER
// Два кадра: руки вверх / руки вниз
//======================================================

#define SEARCH_ROBOT_WHITE   TFT_WHITE
#define SEARCH_ROBOT_BLUE    TFT_GREENYELLOW
#define SEARCH_ROBOT_DARK    0x02A0
#define SEARCH_ROBOT_FACE    TFT_BLACK
#define SEARCH_ROBOT_OUTLINE 0x0010

void drawSearchRobotArm(bool left, bool up)
{
    // Плечо находится прямо у края корпуса робота.
    const int shoulderX = left ? 63 : 97;
    const int shoulderY = 75;

    int elbowX;
    int elbowY;
    int handX;
    int handY;

    if (left)
    {
        elbowX = 52;
        if (up)
        {
            elbowY = 70;
            handX = 44;
            handY = 58;
        }
        else
        {
            elbowY = 81;
            handX = 45;
            handY = 91;
        }
    }
    else
    {
        elbowX = 108;
        if (up)
        {
            elbowY = 70;
            handX = 116;
            handY = 58;
        }
        else
        {
            elbowY = 81;
            handX = 115;
            handY = 91;
        }
    }

    // Верхняя часть руки — толстая светлая линия с тёмным контуром.
    tft.drawLine(shoulderX, shoulderY - 1, elbowX, elbowY - 1, SEARCH_ROBOT_OUTLINE);
    tft.drawLine(shoulderX, shoulderY,     elbowX, elbowY,     SEARCH_ROBOT_OUTLINE);
    tft.drawLine(shoulderX, shoulderY + 1, elbowX, elbowY + 1, SEARCH_ROBOT_OUTLINE);
    tft.drawLine(shoulderX, shoulderY,     elbowX, elbowY,     SEARCH_ROBOT_WHITE);

    // Нижняя часть руки.
    tft.drawLine(elbowX, elbowY - 1, handX, handY - 1, SEARCH_ROBOT_OUTLINE);
    tft.drawLine(elbowX, elbowY,     handX, handY,     SEARCH_ROBOT_OUTLINE);
    tft.drawLine(elbowX, elbowY + 1, handX, handY + 1, SEARCH_ROBOT_OUTLINE);
    tft.drawLine(elbowX, elbowY,     handX, handY,     SEARCH_ROBOT_WHITE);

    // Аккуратные суставы.
    tft.fillCircle(shoulderX, shoulderY, 4, SEARCH_ROBOT_WHITE);
    tft.drawCircle(shoulderX, shoulderY, 4, SEARCH_ROBOT_OUTLINE);
    tft.fillCircle(shoulderX, shoulderY, 2, SEARCH_ROBOT_BLUE);

    tft.fillCircle(elbowX, elbowY, 3, SEARCH_ROBOT_WHITE);
    tft.drawCircle(elbowX, elbowY, 3, SEARCH_ROBOT_OUTLINE);
    tft.fillCircle(elbowX, elbowY, 1, SEARCH_ROBOT_BLUE);

    // Кисть без длинных торчащих пальцев.
    tft.fillCircle(handX, handY, 4, SEARCH_ROBOT_WHITE);
    tft.drawCircle(handX, handY, 4, SEARCH_ROBOT_OUTLINE);
    tft.fillCircle(handX, handY, 2, SEARCH_ROBOT_BLUE);

    // Два коротких пальца в направлении движения руки.
    if (up)
    {
        if (left)
        {
            tft.drawLine(handX - 1, handY - 3, handX - 3, handY - 6, SEARCH_ROBOT_WHITE);
            tft.drawLine(handX + 1, handY - 3, handX + 1, handY - 7, SEARCH_ROBOT_WHITE);
        }
        else
        {
            tft.drawLine(handX + 1, handY - 3, handX + 3, handY - 6, SEARCH_ROBOT_WHITE);
            tft.drawLine(handX - 1, handY - 3, handX - 1, handY - 7, SEARCH_ROBOT_WHITE);
        }
    }
    else
    {
        if (left)
        {
            tft.drawLine(handX - 1, handY + 3, handX - 2, handY + 6, SEARCH_ROBOT_WHITE);
            tft.drawLine(handX + 1, handY + 3, handX + 1, handY + 7, SEARCH_ROBOT_WHITE);
        }
        else
        {
            tft.drawLine(handX + 1, handY + 3, handX + 2, handY + 6, SEARCH_ROBOT_WHITE);
            tft.drawLine(handX - 1, handY + 3, handX - 1, handY + 7, SEARCH_ROBOT_WHITE);
        }
    }
}


void drawSearchNoComponentScreen()
{
    // Тот же зелёный фон, что у экрана поиска, но без робота
    tft.fillRect(2, 20, 156, 90, TFT_DARKGREEN);
    tft.fillRect(4, 22, 152, 86, 0x03E0);
    tft.fillRect(6, 24, 148, 82, 0x0260);

    tft.drawRoundRect(4, 22, 152, 86, 8, TFT_GREENYELLOW);
    tft.drawRoundRect(5, 23, 150, 84, 8, TFT_GREEN);

    // Верхняя рамка поднята на 8 пикселей
    tft.fillRoundRect(18, 36, 124, 28, 8, 0x0320);
    tft.drawRoundRect(18, 36, 124, 28, 8, TFT_GREENYELLOW);

    // Надпись выровнена по вертикали по центру рамки
    drawLocalizedNoComponent(CENTER_X, 44, TFT_WHITE);

    // Нижняя рамка также поднята на 8 пикселей
    tft.fillRoundRect(18, 84, 124, 16, 6, 0x0320);
    tft.drawRoundRect(18, 84, 124, 16, 6, TFT_GREENYELLOW);

    if (useCyrText())
      drawCyrTextCentered("VSTAVTE DETAL", 80, 89, TFT_GREENYELLOW); // ВСТАВЬТЕ ДЕТАЛЬ
    else
      tft.drawCentreString("INSERT PART", 80, 88, 1);
}

static void drawSearchingRobotBody()
{
    // Антенна
    tft.drawFastVLine(80, 31, 5, SEARCH_ROBOT_DARK);
    tft.drawFastVLine(81, 31, 5, SEARCH_ROBOT_DARK);
    tft.fillCircle(80, 28, 4, SEARCH_ROBOT_BLUE);
    tft.drawCircle(80, 28, 4, SEARCH_ROBOT_OUTLINE);
    tft.fillCircle(79, 27, 1, TFT_WHITE);

    // Уши
    tft.fillRoundRect(51, 47, 7, 16, 4, SEARCH_ROBOT_BLUE);
    tft.drawRoundRect(51, 47, 7, 16, 4, SEARCH_ROBOT_OUTLINE);
    tft.fillRoundRect(103, 47, 7, 16, 4, SEARCH_ROBOT_BLUE);
    tft.drawRoundRect(103, 47, 7, 16, 4, SEARCH_ROBOT_OUTLINE);

    // Голова
    tft.fillRoundRect(57, 36, 46, 30, 10, SEARCH_ROBOT_WHITE);
    tft.drawRoundRect(57, 36, 46, 30, 10, SEARCH_ROBOT_OUTLINE);
    tft.fillRoundRect(63, 42, 34, 18, 6, SEARCH_ROBOT_FACE);
    tft.fillRoundRect(70, 47, 6, 8, 3, SEARCH_ROBOT_BLUE);
    tft.fillRoundRect(84, 47, 6, 8, 3, SEARCH_ROBOT_BLUE);
    tft.drawLine(75, 56, 78, 58, SEARCH_ROBOT_BLUE);
    tft.drawLine(78, 58, 82, 58, SEARCH_ROBOT_BLUE);
    tft.drawLine(82, 58, 85, 56, SEARCH_ROBOT_BLUE);

    // Шея и тело
    tft.fillRoundRect(72, 66, 16, 4, 2, SEARCH_ROBOT_BLUE);
    tft.drawRoundRect(72, 66, 16, 4, 2, SEARCH_ROBOT_OUTLINE);
    tft.fillRoundRect(63, 70, 34, 22, 7, SEARCH_ROBOT_WHITE);
    tft.drawRoundRect(63, 70, 34, 22, 7, SEARCH_ROBOT_OUTLINE);
    tft.fillRect(66, 87, 28, 5, SEARCH_ROBOT_BLUE);

    // Экран на груди
    tft.fillRoundRect(69, 75, 22, 11, 3, SEARCH_ROBOT_FACE);
    tft.drawRoundRect(69, 75, 22, 11, 3, SEARCH_ROBOT_OUTLINE);
    tft.drawLine(72, 82, 76, 82, SEARCH_ROBOT_BLUE);
    tft.drawLine(76, 82, 78, 78, SEARCH_ROBOT_BLUE);
    tft.drawLine(78, 78, 80, 84, SEARCH_ROBOT_BLUE);
    tft.drawLine(80, 84, 87, 84, SEARCH_ROBOT_BLUE);

    // Ноги и ступни
    tft.drawLine(73, 92, 71, 100, SEARCH_ROBOT_WHITE);
    tft.drawLine(87, 92, 89, 100, SEARCH_ROBOT_WHITE);
    tft.fillRoundRect(64, 99, 14, 7, 3, SEARCH_ROBOT_WHITE);
    tft.drawRoundRect(64, 99, 14, 7, 3, SEARCH_ROBOT_OUTLINE);
    tft.fillRoundRect(82, 99, 14, 7, 3, SEARCH_ROBOT_WHITE);
    tft.drawRoundRect(82, 99, 14, 7, 3, SEARCH_ROBOT_OUTLINE);
    tft.drawFastHLine(66, 104, 10, SEARCH_ROBOT_BLUE);
    tft.drawFastHLine(84, 104, 10, SEARCH_ROBOT_BLUE);
}

static void clearSearchingRobotArms()
{
    // Очищаем только боковые области, где двигаются руки.
    tft.fillRect(20, 32, 43, 74, 0x0260);
    tft.fillRect(98, 32, 42, 74, 0x0260);

    // Восстанавливаем края декоративной рамки, если рука их задела.
    tft.drawRoundRect(4, 22, 152, 86, 8, TFT_GREENYELLOW);
    tft.drawRoundRect(5, 23, 150, 84, 8, TFT_GREEN);
}

void drawSearchingRobot(bool armsUp)
{
    // Фон рисуется один раз при начале поиска.
    tft.fillRect(2, 20, 156, 90, TFT_DARKGREEN);
    tft.fillRect(4, 22, 152, 86, 0x03E0);
    tft.fillRect(6, 24, 148, 82, 0x0260);
    tft.drawRoundRect(4, 22, 152, 86, 8, TFT_GREENYELLOW);
    tft.drawRoundRect(5, 23, 150, 84, 8, TFT_GREEN);

    drawSearchRobotArm(true, armsUp);
    drawSearchRobotArm(false, armsUp);
    drawSearchingRobotBody();
}

void animateSearchingRobot()
{
    static bool armsUp = false;
    static unsigned long lastAnim = 0;

    if (millis() - lastAnim >= 420)
    {
        armsUp = !armsUp;

        clearSearchingRobotArms();
        drawSearchRobotArm(true, armsUp);
        drawSearchRobotArm(false, armsUp);
        drawSearchingRobotBody();

        lastAnim = millis();
    }
}
