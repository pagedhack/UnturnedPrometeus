/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        WebServer
*
* Created by       :        Arnold Velzel
* Created on       :        28-09-2021
*
*******************************************************************************/
#include <avision_system.h>

#include <avision_debug.h>
#include <avision_storage.h>
#include <avision_files.h>
#include <avision_ws.h>

#include <LittleFS.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

#ifndef __WEB_H__
#define __WEB_H__

namespace AVision {

    class web
    {
        private:

            bool WiFi_Configured;
            bool WiFi_Connected;

            String WiFi_SSID;
            String WiFi_Password;
            String WiFi_Hostname;
            String IP_Address;

            DNSServer dnsServer;
            ESP8266WebServer *server;
            files drive;

            bool useWifi;

            void setWifiConfig();
            AVision::storage WiFiParameters;

            String mimeType(String requestPage);
        public:
            
            void uploadFile();
            void loadPage();
            void getNTPtime();

            void setUploadFileHandler(std::function<void()> handler);
            void setGetPageHandler(std::function<void()> handler);
            void setPostPageHandler(String url, std::function<void()> handler);

            void initPages();

            String _REQUEST(String name);

            void loop();
            void init(AVision::storage WiFisettings);

            web(files systemDrive);
            ~web();
    };
    
}

#endif
