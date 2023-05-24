/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Webserver
*
* Created by       :        Arnold Velzel
* Created on       :        28-09-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_web.h>

using namespace AVision;

String web::_REQUEST(String name)
{
    for (uint8_t i = 0; i < server->args(); i++)
    {
        if (server->argName(i) == name)
        {
            return server->arg(i);
        }
    }
}

String web::mimeType(String requestPage)
{
    String mime = "";
    if (requestPage.indexOf(".html") >= 0)
    {
        mime = "text/html";
    }
    else
    {
        if (requestPage.indexOf(".ico") >= 0)
        {
            mime = "image/x-icon";
        }
        else
        {
            if (requestPage.indexOf(".json") >= 0)
            {
                mime = "application/json";
            }
            else
            {
                if (requestPage.indexOf(".txt") >= 0)
                {
                    mime = "text/plain";
                }
                else
                {
                }
            }
        }
    }
    return mime;
}

void web::uploadFile() 
{
    String requestURI = server->uri();
    HTTPUpload& upload = server->upload();

    yield(); // For large file uploads

    if (upload.status == UPLOAD_FILE_START) {
        dbgln("START");
        dbgln("Upload file: " + upload.filename + " (" + String(upload.totalSize) + ")");
        drive.save("/" + upload.filename, "");
        return;
    }
    if (upload.status == UPLOAD_FILE_WRITE) {
        dbg(".");
        drive.append("/" + upload.filename, String((char*)upload.buf).substring(0, upload.currentSize));
    }
    if (upload.status == UPLOAD_FILE_END) {
        dbgln("END");
        //server->send(200, "text/html", "");
        server->sendHeader("Location", String("/upload.html"), true);
        server->send(302, "text/plain", "");
        return;
    }
    if (upload.status == UPLOAD_FILE_ABORTED) {
        dbgln("ABORT");
        return;
    }
}
void web::loadPage()
{
    String requestURI = server->uri();
    dbgln("Load page: " + requestURI);
    if (requestURI == "/") {
        requestURI = "index.html";
    }
    String requestPage = requestURI;
    String mime = mimeType(requestPage);

    String pageContent = drive.loadPage(requestPage, mime);
    if (pageContent == "")
    {
        if (requestPage.indexOf("connecttest.txt") >= 0) {
            pageContent = "Microsoft Connect Test";
        }
        else 
        {
            if (requestPage.indexOf("ncsi.txt") >= 0) {
                pageContent = "Microsoft NCSI";
            }
            else 
            {
                mime = "text/html";
                pageContent = drive.loadPage("/404.html", mime);
            }
        }
    }
    server->sendHeader("Access-Control-Allow-Origin", "*");
    server->send(200, mime, pageContent);
}
void web::setUploadFileHandler(std::function<void()> handler)
{
    server->onFileUpload(handler);
    setPostPageHandler("/upload/", handler);
}
void web::setGetPageHandler(std::function<void()> handler)
{
    server->onNotFound(handler);
}
void web::setPostPageHandler(String url, std::function<void()> handler)
{
    server->on(url, HTTP_POST, handler);
}

void web::initPages()
{
    server->serveStatic("/manifest.json", LittleFS, "/manifest.json");
    server->serveStatic("/control.html", LittleFS, "/control.html");
    server->serveStatic("/upload.html", LittleFS, "/upload.html");
    server->serveStatic("/setup.html", LittleFS, "/setup.html");
    server->serveStatic("/index.html", LittleFS, "/index.html");
    server->serveStatic("/vue.min.js", LittleFS, "/vue.min.js");
    server->serveStatic("/404.html", LittleFS, "/404.html");
    server->serveStatic("/vue.js", LittleFS, "/vue.js");
    server->serveStatic("/app.js", LittleFS, "/app.js");
    server->serveStatic("/", LittleFS, "/index.html");

    server->begin();
}

void web::loop()
{
    if (WiFi_Configured && !WiFi_Connected) {
        wl_status_t WiFiStatus = WiFi.status();
        if (WiFiStatus == WL_CONNECTED)
        {
            WiFi_Connected = true;
            IP_Address = WiFi.localIP().toString();
            dbgln("IP address:" + IP_Address);
        }
        else
        {
            unsigned long m = millis();
            // dbgln(String(m));dbgln(String(WiFiStatus));delay(500);
            if (m > (60 * 1000)) {
                // not connected within one minute
                dbgln("FAILED to connect to WiFi network, aut reset");
                // Reset wifi settings
                WiFiParameters.set("WiFi_SSID", "");
                WiFiParameters.set("WiFi_Password", "");
                WiFiParameters.loop();
                WiFi_Configured = false;
                ESP.restart();
            }
        }
    }
    dnsServer.processNextRequest();
    server->handleClient();
}

void web::init(AVision::storage WiFisettings)
{
    WiFiParameters = WiFisettings;

    WiFi_SSID = WiFiParameters.get("WiFi_SSID");
    WiFi_Password = WiFiParameters.get("WiFi_Password");
    WiFi_Hostname = WiFiParameters.get("WiFi_Hostname");

    String mac;
    mac = WiFi.macAddress();
    dbgln("MAC address:" + mac);
    mac.replace(":", "");

    if ((WiFi_Hostname.length() > 30) || (WiFi_Hostname.length() == 0))
    {
        WiFi_Hostname = "a-vision-clock";
    }

    WiFi_Configured = (WiFi_SSID.length() <= 30) && (WiFi_Password.length() <= 30) && (WiFi_SSID.length() > 0) && (WiFi_Password.length() > 0);

    if (WiFi_Configured)
    {
        useWifi = true;
        WiFi.begin(WiFi_SSID, WiFi_Password);

        dbgln("Wifi AP:" + WiFi_SSID);
        dbgln("Wifi Host:" + WiFi_Hostname);
    }
    else
    {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++)
        {
            Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
        }

        boolean result = WiFi.softAP("A-VISION-CONFIG-" + mac, "klokklok");
        if (!result)
        {
            dbgln("Failed to set up Wifi ACCESS POINT");
        }

        dnsServer.setTTL(300);
        dnsServer.start(53, "*", WiFi.softAPIP());

        IP_Address = WiFi.softAPIP().toString();
        dbgln("IP address:" + IP_Address);
    }

    if (MDNS.begin(WiFi_Hostname))
    {
        // Add service to MDNS
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("ws", "tcp", WEBSOCKET_PORT);
    }

    initPages();

    dbgln("Webserver initialised");
}

web::web(files systemDrive)
{
    drive = systemDrive;
    useWifi = false;
    WiFi_Configured = false;
    WiFi_Connected = false;
    WiFi_SSID = "";
    WiFi_Password = "";
    WiFi_Hostname = "";
    IP_Address = "???.???.???.???";

    server = new ESP8266WebServer(80);
}

web::~web()
{
}
