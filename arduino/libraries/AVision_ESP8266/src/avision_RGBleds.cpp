/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        RGB LEDs Clock management
*
* Created by       :        Arnold Velzel
* Created on       :        08-09-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_RGBleds.h>

using namespace AVision;

uint8_t RGBleds::secondToLEDindex(int second)
{
    if (LEDreversed)
    {
        return ((60 - second) + firstLED) % LEDcount;
    }
    else
    {
        return (second + firstLED) % LEDcount;
    }
}

uint32_t RGBleds::colorBrightness(uint32_t color, uint8_t brightness)
{
    uint32_t LEDcolor = color;

    uint8_t w = ((LEDcolor >> 24) & 0xFF);
    uint8_t r = ((LEDcolor >> 16) & 0xFF);
    uint8_t g = ((LEDcolor >> 8) & 0xFF);
    uint8_t b = ((LEDcolor >> 0) & 0xFF);

    w = (w * brightness) / 100;
    r = (r * brightness) / 100;
    g = (g * brightness) / 100;
    b = (b * brightness) / 100;

    return (w << 24) + (r << 16) + (g << 8) + b;
}

uint32_t RGBleds::color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
    if (((LEDtype >> 6) & 0b11) == ((LEDtype >> 4) & 0b11))
    {
        // Without white
        int r = (red + white / 2);
        if (r > 255)
            r = 255;
        int g = (green + white / 2);
        if (g > 255)
            g = 255;
        int b = (blue + white / 2);
        if (b > 255)
            b = 255;
        return (r << 16) + (g << 8) + b;
    }
    else
    {
        // With white
        return (white << 24) + (red << 16) + (green << 8) + blue;
    }
}

bool RGBleds::setLED(int index, uint32_t color)
{
    if (CLOCK_LEDS[index] != color)
    {
        CLOCK_LEDS[index] = color;
        UPDATE_LEDS[index] = true;
        return true;
    }
    return false;
}
bool RGBleds::setSecondLED(int second, uint32_t color)
{
    int index = secondToLEDindex(second);
    return setLED(index, color);
}

void RGBleds::setAll(uint32_t color)
{
    for (size_t s = 0; s < LEDcount; s++)
    {
        setLED(s, color);
    }
    update(true);
}
void RGBleds::setAll(uint32_t color, int brightness)
{
    setAll(color);
    setBrightness(brightness);
}

void RGBleds::update(bool force = false)
{
    bool show = false;
    for (size_t i = 0; i < LEDcount; i++)
    {
        if (force || UPDATE_LEDS[i])
        {
            uint32_t color = colorBrightness(CLOCK_LEDS[i], currentBrightness);
            // dbgln(String(i) + " => #" + String(color, 16));
            leds->setPixelColor(i, color);
            show = true;
        }
        UPDATE_LEDS[i] = false;
    }
    if (show)
    {
        leds->show();
    }
}

void RGBleds::setBrightness(int brightness)
{
    if (brightness != currentBrightness)
    {
        currentBrightness = brightness;
        update(true);
    }
}

void RGBleds::setOrientation(int orientation)
{
    firstLED = orientationFirstLED[orientation];
    dbgln("First led: " + String(firstLED));
    update(true);
}

void RGBleds::setLEDcount(uint8_t count)
{
    if (count != LEDcount)
    {
        LEDcount = count;
        orientationFirstLED[0] = 0 * LEDcount / 4;
        orientationFirstLED[1] = 1 * LEDcount / 4;
        orientationFirstLED[2] = 2 * LEDcount / 4;
        orientationFirstLED[3] = 3 * LEDcount / 4;
        if (leds != nullptr) {
            leds->updateLength(LEDcount);
        }
    }
}
void RGBleds::setLEDtype(uint16_t type)
{
    LEDtype = type;
    if (leds != nullptr) {
        leds->updateType(type);
    }
}
void RGBleds::setLEDreversed(bool on)
{
    LEDreversed = on;
}

/*** MODE INIT ***/
void RGBleds::rotate(bool on)
{
    if (on)
    {
        setBrightness(10);
    }
    for (size_t i = 0; i < LEDcount; i++)
    {
        if (on)
        {
            setLED(i, color(255, 255, 255, 255));
        }
        else
        {
            setLED(i, color(0, 0, 0, 0));
        }
        update();
        delay(500 / LEDcount);
    }
    if (!on)
    {
        setBrightness(0);
    }
}
/*** MODE INIT ***/

/*** MODE PARTY ***/
void RGBleds::party(bool on)
{
    if (on)
    {
        if (random(100)%2 == 1) {
            for (size_t i = 0; i < LEDcount; i++)
            {
                uint8_t r = random(255);
                uint8_t g = random(255);
                uint8_t b = random(255);
                setLED(i, color(r, g, b, 0));
            }
            setBrightness(10+random(90));
        }
    }
    else
    {
        setAll(0);
    }
}
/*** MODE PARTY ***/

/*** MODE FIRE ***/
void RGBleds::setPixelHeatColor(int Pixel, byte temperature)
{
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;              // scale up to 0..252

    // figure out which third of the spectrum we're in:
    uint32_t pixel_color = 0xFFFFFF;
    if (t192 > 0x80)
    { // hottest
        pixel_color = color(255, 255, heatramp, 0);
    }
    else if (t192 > 0x40)
    { // middle
        pixel_color = color(255, heatramp, 0, 0);
    }
    else
    { // coolest
        pixel_color = color(heatramp, 0, 0, 0);
    }
    //dbgln(String(Pixel) + " => #" + String(pixel_color, 16));
    setLED(Pixel, pixel_color);
}

void RGBleds::fire()
{
    int Cooling = 50;
    int Sparking = 150;
    int SpeedDelay = 30;
    int cooldown;

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < LEDcount / 2; i++)
    {
        cooldown = random(0, ((Cooling * 10) / (LEDcount / 2)) + 2);

        if (cooldown > heat[i])
        {
            heat[i] = 0;
        }
        else
        {
            heat[i] = heat[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = (LEDcount / 2) - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(255) < Sparking)
    {
        int y = random(7);
        heat[y] = heat[y] + random(160, 255);
        //heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for (int j = 0; j < (LEDcount / 2); j++)
    {
        setPixelHeatColor(j, heat[j]);
        setPixelHeatColor(LEDcount - j - 1, heat[j]);
    }

    update(true);
    delay(SpeedDelay);
}
/*** MODE FIRE ***/

void RGBleds::loop()
{
    update();
}

void RGBleds::init(int pin)
{
    leds = new Adafruit_NeoPixel(LEDcount, pin, LEDtype + NEO_KHZ800);
    leds->begin();
    leds->show();
    for (size_t i = 0; i < LEDcount; i++)
    {
        setLED(i, 0);
    }
    update(true);
    initialised = true;

    dbgln("RGBleds initialised");
}
void RGBleds::init()
{
    init(LED_DIN_PIN);
}

RGBleds::RGBleds()
{
    leds = nullptr;
    LEDtype = LED_TYPE;
    LEDcount = LED_COUNT;
    LEDreversed = LED_REVERSED;
    currentBrightness = 0;
}

RGBleds::~RGBleds()
{
}
