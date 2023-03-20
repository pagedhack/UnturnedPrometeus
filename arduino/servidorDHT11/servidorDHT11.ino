#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebSrv.h>

#include "DHTesp.h"
#define DHTpin 15

const char* ssid="RedAnonima_2.4G";
const char* password="Competitiva2";

AsyncWebServer server(80);
DHTesp dht;

void conectarse(){
  Serial.println();
  Serial.println();
  Serial.print("Conenecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("#");
  }
  Serial.println("");
  Serial.println("Wifi Connected");
  Serial.println("Ip Address: ");
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  // Serial.println("Estado\tHumedad (%)\tTemperatura (C)\t(F)\tIndiceCalor (C)\t(F)");
  // dht.setup(DHTpin, DHTesp::DHT11);

  conectarse();

  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest* r){
    dht.setup(DHTpin, DHTesp::DHT11);
    String estado = dht.getStatusString();
    r->send(200, "text/html", "<h1>Humedad - (%)\tTemperatura - (C)\t(F)\tIndiceCalor (C)\t(F)</h1>");
  });

  server.on("/estado", HTTP_GET, [](AsyncWebServerRequest* r){
    dht.setup(DHTpin, DHTesp::DHT11);
    String estado = dht.getStatusString();
    r -> send(200, "text/html", "<h1> Estado: <br>" + estado + "</h1>");
  });

  server.on("/humedad", HTTP_GET, [](AsyncWebServerRequest* r){
    dht.setup(DHTpin, DHTesp::DHT11);
    float humedad =  dht.getHumidity();
    String hum = "";
    hum.concat(humedad);
    r -> send(200, "text/html", "<h1> Humedad (C°): " + hum + "</h1> <br><br> <h1>Humedad (F°): " + hum + "</h1>");
  });

  server.on("/temperatura", HTTP_GET, [](AsyncWebServerRequest* r){
    dht.setup(DHTpin, DHTesp::DHT11);
    float temperatura = dht.getTemperature();
    String tem = "";
    tem.concat(temperatura);
    r -> send(200, "text/html", "<h1> Temperatura (C°): " + tem + "</h1>");
  });

  server.begin();
}

void loop(){
  delay(dht.getMinimumSamplingPeriod());

  float humedad =  dht.getHumidity();
  float temperatura = dht.getTemperature();                            

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("No se pudo leer sensor DHT!");
    return;

  }

  Serial.print( dht.getStatusString() );
  Serial.print("\t");
  Serial.print(humedad, 1);
  Serial.print("\t\t");
  Serial.print(temperatura, 1);
  Serial.print("\t\t");
  Serial.print( dht.toFahrenheit(temperatura), 1);
  Serial.print("\t\t");
  Serial.print( dht.computeHeatIndex(temperatura, humedad, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperatura), humedad, true), 1);
  delay(2000);

}

