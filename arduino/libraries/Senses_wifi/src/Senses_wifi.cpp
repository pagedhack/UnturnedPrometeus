#include "Senses_wifi.h"

#define SENSES_HOST "www.sensesiot.com"
#define SENSES_PORT 4000
#define retry_wait 500
#define LAGTIME 100

#define POSTPATH "http://www.sensesiot.com:4003/wfev"
#define WFEV_PORT 4003
#define wfev_retry_wait 1000
#define MAXWIFINETWORK 6
#define WAITTIMESCAN 300

String Senses_wifi::connect(const char *ssid, const char *passw, const char *userid, const char *key){

  _ssid = ssid;
  _passw = passw;
  _userid = userid;
  _key = key;

  WiFi.begin(_ssid,_passw);
  Serial.print("SENSES platform start connecting.");

  while (WiFi.status() != WL_CONNECTED){
    delay(retry_wait);
    Serial.print(".");
  }

  if(WiFi.status() == WL_CONNECTED){

    /* - WiFi environment - */
    String macPayload = "{\"key\":\"" + String(_key) + "\",\"datasets\":[";
    int numWiFiAP = WiFi.scanNetworks();
    delay(WAITTIMESCAN);
      if(numWiFiAP > MAXWIFINETWORK){
        numWiFiAP = MAXWIFINETWORK;
      }

    byte bssid[MAXWIFINETWORK];
    for(int i=0; i<numWiFiAP; i++){
      macPayload += "{\"macAddress\":\"";
      macPayload += MACtoString(WiFi.BSSID(i));
      macPayload += "\",";
      macPayload += "\"rssi\":\"";
      macPayload += WiFi.RSSI(i);
      macPayload += "\",";
      macPayload += "\"channel\":\"";
      macPayload += WiFi.channel(i);
      macPayload += "\"}";
    if(i < numWiFiAP-1){
      macPayload += ",";
    }else{
      macPayload += "]}";
    }
  }

  HTTPClient http;
  if(WiFi.status() == WL_CONNECTED){
    http.begin(POSTPATH);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(macPayload);
    _response = http.getString();
    http.end();
  }else{
    _response = "Cannot connected to server";
  }

    delay(2000);

    /* SENSES server ready check */
    WiFiClient client;
    if(client.connect(SENSES_HOST, SENSES_PORT)){
      _path = "GET /";
      _path += " HTTP/1.1\r\n";
      _path += "Host: ";
      _path += SENSES_HOST;
      _path += ":";
      _path += SENSES_PORT;
      _path += "\r\n";
      _path += "Connection: keep-alive\r\n\r\n";

      client.print(_path);
      delay(LAGTIME);

      while(client.available()){
        _response = client.readStringUntil('\r');
      }

      return _response;

    }
  }

}

String Senses_wifi::send(int slotnum, float data){

  _slotnum = slotnum;
  _data = data;

  WiFiClient client;
  if(client.connect(SENSES_HOST, SENSES_PORT)){

    _path = "GET /send/";
    _path += String(_userid);
    _path += "/";
    _path += String(_key);
    _path += "/";
    _path += String(_slotnum);
    _path += "/";
    _path += String(_data);

    _path += " HTTP/1.1\r\n";
    _path += "Host: ";
    _path += SENSES_HOST;
    _path += ":";
    _path += SENSES_PORT;
    _path += "\r\n";
    _path += "Connection: keep-alive\r\n\r\n";

    client.print(_path);
    delay(LAGTIME);

    while(client.available()){
      _response = client.readStringUntil('\r');
    }

    return _response;
  }
}

String Senses_wifi::getDigitalControl(int controlport){

  _controlport = controlport;

  WiFiClient client;
  if(client.connect(SENSES_HOST, SENSES_PORT)){

    _path = "GET /getdigitalstatus/";
    _path += String(_userid);
    _path += "/";
    _path += String(_key);
    _path += "/";
    _path += String(_controlport);

    _path += " HTTP/1.1\r\n";
    _path += "Host: ";
    _path += SENSES_HOST;
    _path += ":";
    _path += SENSES_PORT;
    _path += "\r\n";
    _path += "Connection: keep-alive\r\n\r\n";

    client.print(_path);
    delay(LAGTIME);

    while(client.available()){
      _controlresponse = client.readStringUntil('\r');
    }

    _controlresponse.replace("\n","");

    return String(_controlresponse);
  }
}

String Senses_wifi::MACtoString(uint8_t* macAddress) {
    uint8_t mac[6];
    char macStr[18] = { 0 };
    #ifdef ARDUINO_ARCH_SAMD
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[5], macAddress[4], macAddress[3], macAddress[2], macAddress[1], macAddress[0]);
    #elif defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
    #endif
    return  String(macStr);
}
