/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Dot-matrix display
*
* Created by       :        Arnold Velzel
* Created on       :        24-09-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_display.h>

using namespace AVision;

void dotmatrixDisplay::showDot(uint8_t r, uint16_t c)
{
    if (!display->getPoint(r, c))
    {
        display->setPoint(r, c, true);
    }
}
void dotmatrixDisplay::hideDot(uint8_t r, uint16_t c)
{
    if (display->getPoint(r, c))
    {
        display->setPoint(r, c, false);
    }
}
void dotmatrixDisplay::flashDot(uint8_t r, uint16_t c, int timeOn = 100, int timeOff = 900)
{
    static unsigned int last_toggle = 0;
    static bool state = false;
    unsigned int m = millis();
    if (state && (m - last_toggle) > timeOn)
    {
        state = false;
        hideDot(r, c);
        last_toggle = m;
    }
    if (!state && (m - last_toggle) > timeOff)
    {
        state = true;
        showDot(r, c);
        last_toggle = m;
    }
}

void dotmatrixDisplay::setRow(uint8_t row, uint8_t value)
{
    display->setRow(row, value);
}
void dotmatrixDisplay::setColumn(uint8_t column, uint8_t value)
{
    display->setColumn(column, value);
}

void dotmatrixDisplay::printText(uint8_t modStart, uint8_t modEnd, char *text)
{
    char c;
    int16_t col = modStart * COL_SIZE;
    uint16_t len = 0;
    uint16_t size = 0;
    uint8_t cBuf[8];
    uint8_t data;
    bool tiny = false;

    display->control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

    do
    {
        if (size == 0) {
            c = *text;
            if (c == '▼')
            {
                tiny = true;
                text++;
                c = *text;
            }
            if (c == '▲')
            {
                tiny = false;
                text++;
                c = *text;
            }
            if (c == '\0') {
                size = 1;
            } else {
                if (tiny && c >= 42 && c <= 62)
                {
                    c += 100;
                }
                text++;
                len = display->getChar(c, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
                size = len;
            }
        }
        if (size > 0) {
            data = 0x00;
            if (c != '\0') {
                data = cBuf[len - size];
            }
            display->setColumn(col++, data);
            size--;
            if (size == 0 && c != ' ' && c != '\0') {
                size = CHAR_SPACING;
                c = '\0';
            }
        }
    } while (col < (modEnd+1) * COL_SIZE);    

    display->control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void dotmatrixDisplay::setIntensity(uint8_t intensity, uint8_t startDev = 0, uint8_t endDev = 7)
{
    static uint8_t currentIntensity = 101;
    if (intensity != currentIntensity || currentIntensity > 100)
    {
        display->control(startDev, endDev, MD_MAX72XX::INTENSITY, intensity * MAX_INTENSITY / 100);
        currentIntensity = intensity;
    }
}
void dotmatrixDisplay::setIntensity(uint8_t intensity)
{
    setIntensity(intensity, 0, 7);
}
void dotmatrixDisplay::shiftLeft()
{
    display->transform(MD_MAX72XX::TSR);
}

void dotmatrixDisplay::invert(uint8_t startDev, uint8_t endDev)
{
    display->transform(startDev, endDev, MD_MAX72XX::TINV);
}
void dotmatrixDisplay::setFont(MD_MAX72XX::fontType_t *f)
{
    display->setFont(f);
}

void dotmatrixDisplay::loop()
{
    const char *t = text.c_str();
    size_t row = 0;
    if (text.indexOf('\n') < 0) {
        printText(0, (display_rows * display_columns) - 1, (char *)t);
    } else {
        char *part;
        part = strtok((char *)t, "\n");
        while (part != nullptr)
        {
            printText(row * display_columns, (row * display_columns + display_columns) - 1, part);
            part = strtok(NULL, "\n");
            row++;
        }
    }
}
void dotmatrixDisplay::init(int columns, int rows, MD_MAX72XX::moduleType_t hardware_type)
{
    display_columns = columns;
    display_rows = rows;

    // SPI hardware interface
    display = new MD_MAX72XX(hardware_type, DATA_PIN, CLK_PIN, CS_PIN, display_columns * display_rows);
    display->begin();
    display->setFont(_font_with_tiny_numbers);

    setIntensity(10, 0, display_columns * display_rows);

    dbgln("Display initialised");
}
void dotmatrixDisplay::init(int columns, int rows)
{
    init(columns, rows, DISPLAY_HARDWARE_TYPE);
}
dotmatrixDisplay::dotmatrixDisplay()
{
    display_columns = 1;
    display_rows = 1;
    display_brightness = 1;
    text = "";
}

dotmatrixDisplay::~dotmatrixDisplay()
{
}