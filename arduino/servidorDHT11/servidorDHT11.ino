#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebSrv.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
// #include <Adafruit_Sensor.h>

#include "DHTesp.h"
#define DHTpin 15

//Provide the token generation process info.
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "neues Doppelnetz"
#define WIFI_PASSWORD "13161912"

// Insert Firebase project API Key
#define API_KEY "AIzaSyA15mpyOJuNTlJCTQ_mmUsLIv8JVuoBdys"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "1@gmail.com"
#define USER_PASSWORD "123456789"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://dht11-4a3c7-default-rtdb.firebaseio.com/" 

// const char* WIFI_SSID="RedAnonima_2.4G";
// const char* WIFI_PASSWORD="Competitiva2";

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;
String tempPath;
String humPath;

  // float humedad =  dht.getHumidity();
  // float temperatura = dht.getTemperature(); 

DHTesp dht;
String estado;

// delay(dht.getMinimumSamplingPeriod());
// dht.setup(DHTpin, DHTesp::DHT11);
float temperatura = dht.getTemperature();
float humedad = dht.getHumidity();
float contador = 1.0;

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 5000;


// AsyncWebServer server(80);
// DHTesp dht;

void conectarse(){
  Serial.println();
  Serial.print("Conenecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("#");
  }
  Serial.println("");
  Serial.println("Wifi Connected");
  Serial.println("Ip Address: ");
  Serial.println(WiFi.localIP());
}

void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println();

  conectarse();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid;

  // Update database path for sensor readings
  tempPath = databasePath + "/temperatura"; // --> UsersData/<user_uid>/temperature
  humPath = databasePath + "/humedad"; // --> UsersData/<user_uid>/humidity
  // presPath = databasePath + "/pressure"; // --> UsersData/<user_uid>/pressure

  

  //viejo
  // server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest* r){
  //   dht.setup(DHTpin, DHTesp::DHT11);
  //   String estado = dht.getStatusString();
  //   r->send(200, "text/html", "<h1>Humedad - (%)\tTemperatura - (C)\t(F)\tIndiceCalor (C)\t(F)</h1>");
  // });

  // server.on("/estado", HTTP_GET, [](AsyncWebServerRequest* r){
  //   dht.setup(DHTpin, DHTesp::DHT11);
  //   String estado = dht.getStatusString();
  //   r -> send(200, "text/html", "<h1> Estado: <br>" + estado + "</h1>");
  // });

  // server.on("/humedad", HTTP_GET, [](AsyncWebServerRequest* r){
  //   dht.setup(DHTpin, DHTesp::DHT11);
  //   float humedad =  dht.getHumidity();
  //   String hum = "";
  //   hum.concat(humedad);
  //   r -> send(200, "text/html", "<h1> Humedad (C°): " + hum + "</h1> <br><br> <h1>Humedad (F°): " + hum + "</h1>");
  // });

  // server.on("/temperatura", HTTP_GET, [](AsyncWebServerRequest* r){
  //   dht.setup(DHTpin, DHTesp::DHT11);
  //   float temperatura = dht.getTemperature();
  //   String tem = "";
  //   tem.concat(temperatura);
  //   r -> send(200, "text/html", "<h1> Temperatura (C°): " + tem + "</h1>");
  // });

  // server.begin();
}



void loop(){

  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

  if(contador>20){contador = 1.0;}
    contador=contador+1;
    temperatura = contador*0.1;
    humedad = contador*0.2;
    // pressure = contador*0.3;


    // Send readings to database:
    sendFloat(tempPath, temperatura);
    sendFloat(humPath, humedad);
    // sendFloat(presPath, pressure);
  }

  //viejo
  // delay(dht.getMinimumSamplingPeriod());

  // float humedad =  dht.getHumidity();
  // float temperatura = dht.getTemperature();                            

  // if (isnan(humedad) || isnan(temperatura)) {
  //   Serial.println("No se pudo leer sensor DHT!");
  //   return;

  // }

  // Serial.print( dht.getStatusString() );
  // Serial.print("\t");
  // Serial.print(humedad, 1);
  // Serial.print("\t\t");
  // Serial.print(temperatura, 1);
  // Serial.print("\t\t");
  // Serial.print( dht.toFahrenheit(temperatura), 1);
  // Serial.print("\t\t");
  // Serial.print( dht.computeHeatIndex(temperatura, humedad, false), 1);
  // Serial.print("\t\t");
  // Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperatura), humedad, true), 1);
  // delay(2000);

}

