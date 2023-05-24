/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        File storage handler
*
* Created by       :        Arnold Velzel
* Created on       :        27-09-2021
*
*******************************************************************************/
#include <avision_system.h>

// https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases
#include <LittleFS.h>

#ifndef __FILES_H__
#define __FILES_H__

namespace AVision {

    class files
    {
        private:

            String line(String contents, int nr, char separator);
            String line(String contents, int nr);
            
        public:
            bool remove(String path);
            bool append(String path, String contents);
            bool save(String path, String contents);

            String load(String path);
            String loadPage(String path, String mime);
            String loadPage(String path);

            void list();

            void loop();
            void init();

            files();
            ~files();
    };

}

#endif