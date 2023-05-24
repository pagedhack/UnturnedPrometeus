/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Dotmatrix display management
*
* Created by       :        Arnold Velzel
* Created on       :        08-09-2021
*
*******************************************************************************/
#include <avision_system.h>

// https://github.com/MajicDesigns/MD_MAX72XX
#include <SPI.h>
#include <MD_MAX72xx.h>

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

// https://pjrp.github.io/MDParolaFontEditor
#include <avision_font_with_tiny_numbers.h>

#define AVISION_DISPLAY         MD_MAX72XX::DR0CR1RR1_HW

// Define the number of devices we have in the chain and the hardware interface
#ifndef DISPLAY_HARDWARE_TYPE
#define DISPLAY_HARDWARE_TYPE   AVISION_DISPLAY
#endif

#define CLK_PIN                 D5  // or SCK
#define DATA_PIN                D7  // or MOSI
#define CS_PIN                  D8  // or CS

#define CHAR_SPACING            1 // pixels between characters

namespace AVision
{
    class dotmatrixDisplay
    {
        private:

            MD_MAX72XX *display;
            uint8_t display_brightness;

            void printText(uint8_t start, uint8_t end, char *text);

            int display_rows;
            int display_columns;

        public:
            String text; // Use ▼ to use tiny numbers and ▲ to use normal sized numbers

            void setRow(uint8_t row, uint8_t value);
            void setColumn(uint8_t column, uint8_t value);

            void showDot(uint8_t row, uint16_t column);
            void hideDot(uint8_t row, uint16_t column);
            void flashDot(uint8_t row, uint16_t column, int on, int off);
            void invert(uint8_t start, uint8_t end);
            void shiftLeft();

            void setIntensity(uint8_t intensity, uint8_t start, uint8_t end);
            void setIntensity(uint8_t intensity); // whole display
            void setFont(MD_MAX72XX::fontType_t *f);
            void loop();
            void init(int columns, int rows, MD_MAX72XX::moduleType_t hardware_type);
            void init(int columns, int rows);

            dotmatrixDisplay();
            ~dotmatrixDisplay();
    };
}

#endif
