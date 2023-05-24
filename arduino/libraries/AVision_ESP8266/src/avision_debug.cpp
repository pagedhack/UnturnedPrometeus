/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Debug
*
* Created by       :        Arnold Velzel
* Created on       :        26-09-2021
*
*******************************************************************************/

#include <avision_debug.h>

bool debug_active = false;

void dbg(String s)
{
    if (debug_active) {
        Serial.print(s);
    }
}
void dbgln(String s)
{
    if (debug_active) {
        dbg(s);
        Serial.println();
    }
}

void initDebug(bool active)
{
    debug_active = active;

    Serial.begin(SERIAL_BAUDRATE);
    delay(1000);
    Serial.println("\e[3J"); // Clear screen

    delay(100);
    Serial.println("\n\n\n");
    Serial.println("2021 © Copyright\n");
    Serial.flush();
    Serial.println(" /\\__\\  /o _o _ ._ ");
    Serial.println("/--\\  \\/ |_>|(_)| |");
    Serial.println("");
    Serial.println(" __");
    Serial.println("(_  _ |  -|-o _ ._  _");
    Serial.println("__)(_)||_||_|(_)| |_>");
    Serial.println("\n\n\n");
    Serial.flush();
    delay(100);
}
