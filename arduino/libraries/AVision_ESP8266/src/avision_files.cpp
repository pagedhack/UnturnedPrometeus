/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        File handler
*
* Created by       :        Arnold Velzel
* Created on       :        31-08-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_files.h>

using namespace AVision;

bool files::remove(String path)
{
    if (LittleFS.exists(path)) {
        bool success = LittleFS.remove(path);
        return success;
    }
    return true;
}

bool files::append(String path, String contents)
{
    File f = LittleFS.open(path, "a");
    if (f)
    {
        int bytesWritten = f.print(contents);
        f.close();
        if (bytesWritten == 0) {
            return false;
        }
        return true;
    }
    return false;
}

bool files::save(String path, String contents)
{
    File f = LittleFS.open(path, "w");
    if (f)
    {
        int bytesWritten = f.print(contents);
        if (bytesWritten == 0) {
            return false;
        }
        f.close();
        return true;
    }
    return false;
}

String files::load(String path)
{
    String contents = "";

    File f = LittleFS.open(path, "r");
    if (f)
    {
        f.setTimeout(100);
        while (f.available())
        {
            contents = contents + String((char)f.read());
            ESP.wdtFeed();
        }
        f.close();
    }
    else
    {
    }
    return contents;
}

String files::line(String contents, int nr, char separator = '\n')
{
    int _start = 0;
    int _end = 0;
    while (_end >= 0 && nr)
    {
        if (_end) {
            _start = _end + 1;
        }
        _end = contents.indexOf(separator, _start);
        nr--;
    }
    if (_end >= 0)
    {
        return contents.substring(_start, _end);
    }
    return "";
}

String files::loadPage(String path, String mime = "text/html")
{
    if (mime.indexOf("text") >= 0 || mime.indexOf("json") >= 0)
    {
        return load(path);
    }
    return "";
}

void files::list()
{
    delay(100);
    Serial.println("\nDirectory list -");
    Dir dir = LittleFS.openDir("/");
    delay(500);
    while (dir.next()) {
        Serial.print(dir.fileName());
        if(dir.fileSize()) {
            File f = dir.openFile("r");
            Serial.println(" (" + String(f.size()) + ")");
            f.close();
        } else {
            Serial.println();
        }
        delay(10);
        ESP.wdtFeed();
    }
}

void files::loop()
{
    //
}
void files::init()
{
    LittleFS.begin();
    dbgln("Files initialised");
}

files::files()
{
}

files::~files()
{
}
