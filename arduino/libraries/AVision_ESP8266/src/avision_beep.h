/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Buzzer
*
* Created by       :        Arnold Velzel
* Created on       :        24-09-2021
*
*******************************************************************************/
#include <avision_system.h>

#ifndef BEEP_BUZZER
#define BEEP_BUZZER     D0
#endif

#ifndef __BEEP_H__
#define __BEEP_H__

#define BEEP_FREQUENCY  4000

void beep(unsigned int frequency, int milliseconds, int pause);
void beep(int milliseconds, int pause);

void initBeep(int pin);
void initBeep();

#endif
