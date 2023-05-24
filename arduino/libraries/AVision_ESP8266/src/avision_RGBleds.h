/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        RGB leds
*
* Created by       :        Arnold Velzel
* Created on       :        27-09-2021
*
*******************************************************************************/
#include <avision_system.h>

#include <Adafruit_NeoPixel.h>

#include <avision_debug.h>

#ifndef LED_DIN_PIN
#define LED_DIN_PIN D3
#endif
#ifndef LED_TYPE
#define LED_TYPE    NEO_GRB
#endif
#ifndef LED_COUNT
#define LED_COUNT   60
#endif
#define LED_COUNT_4 LED_COUNT / 4
#define LED_REVERSED false

#ifndef __LEDS_H__
#define __LEDS_H__

namespace AVision
{
    class RGBleds
    {
        private:
            int orientationFirstLED[4] = {0, LED_COUNT_4, LED_COUNT_4 * 2, LED_COUNT_4 * 3}; // UP, RIGHT, DOWN, LEFT
            int firstLED = 0;
            int currentBrightness;

            // https://github.com/adafruit/Adafruit_NeoPixel (Adafruit NeoPixel)
            // https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
            uint32_t CLOCK_LEDS[LED_COUNT];
            bool UPDATE_LEDS[LED_COUNT];

            Adafruit_NeoPixel *leds;

            uint8_t secondToLEDindex(int second);
            uint32_t colorBrightness(uint32_t color, uint8_t brightness);
            int LEDcount;
            int LEDtype;
            bool LEDreversed;

            byte heat[100];
            void setPixelHeatColor (int Pixel, byte temperature);

            void update(bool force);
        public:
            void rotate(bool on);
            void party(bool on);
            void fire();

            bool initialised = false;

            uint32_t color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white); // RED, GREEN, BLUE, WHITE (0..255)
            bool setLED(int index, uint32_t color);
            bool setSecondLED(int second, uint32_t color);
            void setAll(uint32_t color);
            void setAll(uint32_t color, int brightness);
            void setBrightness(int brightness);
            void setOrientation(int orientation);
            void setLEDcount(uint8_t count);
            void setLEDtype(uint16_t type);
            void setLEDreversed(bool on);

            void loop();
            void init(int pin);
            void init();

            RGBleds();
            ~RGBleds();
    };

}

#endif
