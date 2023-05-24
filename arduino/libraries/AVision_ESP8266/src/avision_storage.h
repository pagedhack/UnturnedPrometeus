/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        EEPROM data storage handler
*
* Created by       :        Arnold Velzel
* Created on       :        27-09-2021
*
*******************************************************************************/
#include <avision_system.h>

#include <EEPROM.h>

#ifndef __STORAGE_H__
#define __STORAGE_H__

struct EEPROMsetting {
    int address;
    int length;
    String name;
    String value;
};

namespace AVision {

    class storage
    {
        private:

            EEPROMsetting settings[4] = {
                { 0, 32, "WiFi_SSID", "" },
                { 32, 32, "WiFi_Password", "" },
                { 64, 32, "WiFi_Hostname", "" },
                { 0, 0} // Terminator
            };

            bool dirty = false;
            String EEPROMreadWord(int address);
            String EEPROMreadWord(int address, int maxSize);
            void EEPROMwriteWord(int address, String word);

            void read();
            void write();

        public:

            String get(String name);
            void set(String name, String value);

            void loop();
            void init();

            storage();
            ~storage();
    };
        
}

#endif
