#include <WiFi.h>
#include <ESPAsyncwebserver.h>

const char* ssid="dr-rojano";
const char* password="78928-tw";

Async webserver server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Conectarse();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)){
    int numParametros= request->send();
    Serial.printf(numParametros);

    if(numParametros == 0){
      request->send(200, "text/html", "<h1>hola mundo</h1>");
    }
    else{
      AsyncWebServer *p = request->getParam(0);
      String html = "<h1>hola" + xp->value() + "desde Esp32</h1>"
      request->send(200, "text/html", + html);
    }
  };

  server.on("/adios", HTTP_GET, [](AsyncWebServerRequest *r)){
    r -> send(200, "text/html", "<h1>adios</h1>");
  };

  server.begin();
}

 
void loop() {
  // put your main code here, to run repeatedly:

}
