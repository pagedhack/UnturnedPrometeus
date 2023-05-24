#ifndef Senses_wifi_h
#define Senses_wifi_h

#include "Arduino.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"

class Senses_wifi{
  public:
    String connect(const char *ssid, const char *passw, const char *userid, const char *key);
    String send(int slotnum, float data);
    String MACtoString(uint8_t* macAddress);
    String getDigitalControl(int controlport);
    const char *_ssid, *_passw;
    const char *_userid, *_key;
    String _controlresponse;

  private:
    int _slotnum;
    float _data;
    String _path;
    String _response;
    String _wfMcaddr;
    int _controlport;
};

#endif
