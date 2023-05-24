/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        EEPROM data handler
*
* Created by       :        Arnold Velzel
* Created on       :        27-09-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_storage.h>

using namespace AVision;

void storage::EEPROMwriteWord(int address, String word)
{
    delay(10);

    for (int i = 0; i < word.length(); ++i)
    {
        EEPROM.write(address + i, word[i]);
    }

    EEPROM.write(address + word.length(), '\0');
    EEPROM.commit();
}

String storage::EEPROMreadWord(int address, int maxSize = 32)
{
    String word;
    char readChar = '?';
    int i = address;

    while (readChar != '\0' && word.length() < maxSize)
    {
        readChar = char(EEPROM.read(i));
        delay(10);
        i++;

        if (readChar != '\0')
        {
            word += readChar;
        }
        ESP.wdtFeed();
    }

    return word;
}
String storage::EEPROMreadWord(int address)
{
    return EEPROMreadWord(address, 32);
}

void storage::read()
{
    size_t s = 0;
    do
    {
        settings[s].value = EEPROMreadWord(settings[s].address, settings[s].length);
        s++;
        ESP.wdtFeed();
    } while (settings[s].length);
    dirty = false;
}
void storage::write()
{
    size_t s = 0;
    do
    {
        EEPROMwriteWord(settings[s].address, settings[s].value);
        s++;
        ESP.wdtFeed();
    } while (settings[s].length);
    dirty = false;

    dbgln("Storage updated.");
}

String storage::get(String name)
{
    size_t s = 0;
    do
    {
        if (settings[s].name == name)
        {
            return settings[s].value;
        }
        s++;
        ESP.wdtFeed();
    } while (settings[s].length);
    return "";
}
void storage::set(String name, String value)
{
    size_t s = 0;
    do
    {
        if (settings[s].name == name)
        {
            if (settings[s].value != value)
            {
                dbgln(name + " = " + value);
                settings[s].value = value;
                dirty = true;
            }
            return;
        }
        s++;
        ESP.wdtFeed();
    } while (settings[s].length);
}

void storage::loop()
{
    if (dirty)
    {
        write();
    }
}

void storage::init()
{
    EEPROM.begin(512); //Initialize EEPROM

    read();
    dbgln("Storage  initialised");
}

storage::storage()
{
}

storage::~storage()
{
}
