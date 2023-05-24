/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Buzzer functions
*
* Created by       :        Arnold Velzel
* Created on       :        24-09-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_beep.h>

int buzzer_pin = BEEP_BUZZER;

void beep(unsigned int frequency, int milliseconds, int pause)
{
    if (milliseconds)
    {
        tone(buzzer_pin, frequency, milliseconds);
    }
    if (pause)
    {
        delay(milliseconds + pause);
        noTone(buzzer_pin);
        digitalWrite(buzzer_pin, LOW);
    }
}
void beep(int milliseconds, int pause = 0)
{
    beep(BEEP_FREQUENCY, milliseconds, pause);
}

void initBeep(int pin)
{
    buzzer_pin = pin;
    pinMode(buzzer_pin, OUTPUT);
    digitalWrite(buzzer_pin, LOW);
    dbgln("Beep initialised");
}
void initBeep()
{
    initBeep(BEEP_BUZZER);
}
