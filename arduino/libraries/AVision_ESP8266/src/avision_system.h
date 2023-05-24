/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Generic A-Vision system definitions
*
* Created by       :        Arnold Velzel
* Created on       :        26-09-2021
*
*******************************************************************************/
#include <Arduino.h>

#ifndef __AVISION_SYSTEM_H__
#define __AVISION_SYSTEM_H__

/*** GENERIC CALLBACK FUNCTION TYPEDEF ***/
typedef void (*callbackFunction)();
typedef void (*callbackFunctionInt)(int);
typedef void (*callbackFunctionBool)(bool);
typedef void (*callbackFunctionString)(String);

#endif
