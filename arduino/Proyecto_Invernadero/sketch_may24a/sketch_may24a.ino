/**
 * Librerias de WiFi y utilidades para publicar en MQTT
*/

#include <WiFi.h>
#include <AsyncTCP.h>
//#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient

//Para el sensor de Temperatura DHT11 y DHT22
#include "DHTesp.h" // http://librarymanager/All#DHTesp

#include <Arduino_JSON.h> //https://github.com/arduino-libraries/Arduino_JSON

#define DHTpin 15

DHTesp dht;

//Topics MQTT
const char* topic_temp = "646e39ec5c093741fd3a7efd/temperatura";
const char* topic_hum = "646e39ec5c093741fd3a7efd/humedad";
// Constantes de configuracion
const char* ssid = "Doppelnetz";              //Nombre Red Wi-Fi
const char* password = "13161912";           //Contrasenia Red Wi-Fi

const char* mqtt_server = "mqtt.mikrodash.com"; //Servidor MQTT
const char* mqtt_server1 = "192.168.137.1"; //Servidor MQTT

const int mqtt_port = 1883;                     //Puerto MQTT
const int qos_level=1;                          //Calidad del servicio MQTT

//Credenciales MQTT MikroDash
const char* usernameMQTT = "esp32_1685037382188";
const char* passwordMQTT = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhY2wiOiI0IiwiZXhwIjoxNjg3NTY0NzI0LCJpYXQiOjE2ODUwMzc1MjQsInVzZXJfaWQiOiI2NDZlMzllYzVjMDkzNzQxZmQzYTdlZmQiLCJ1c2VybmFtZSI6IiJ9.xdic9C_79LPyOkTMh8WnADbgJiIjQTaVhKZ8oYPfaTs";

//Variables para la conexión y envio de mensajes MQTT
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (150)
char msg[MSG_BUFFER_SIZE];

int value = 0;

//Conexion Wi-Fi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

// Funcion para reconectar
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    // Se genera un ID aleatorio con
    String clientId = "MikroDashWiFiClient-";
    //clientId += String(random(0xffff), HEX);

    //Cuando se conecta
    if (client.connect(clientId.c_str(),usernameMQTT,passwordMQTT) ) {
      Serial.println("Conectado");

      //Topics a los que se suscribira para recibir cambios
        //client.publish("invernadero/IOT", "temperatura");
      client.subscribe(topic_temp, qos_level);
      //client.subscribe(topic_pwm,qos_level);
      //client.subscribe(topic_led,qos_level);
    } else {
      Serial.print("Error al conectar: ");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void setup(){
  dht.setup(DHTpin, DHTesp::DHT11); // Cambiar DHT11 por DHT22 sino obtienes datos
  
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);      //Comunicación serial para monitorear
  setup_wifi();               //Se llama la funcición para conectarse al Wi-Fi
  client.setServer(mqtt_server, mqtt_port);  //Se indica el servidor y puerto de MQTT para conectarse
}

bool first = true;

void loop()
{
  if (!client.connected()) { //Si se detecta que no hay conexión con el broker 
    reconnect();              //Reintenta la conexión
  }
  client.loop();            //De lo contrario crea un bucle para las suscripciones MQTT

  float temperature = 24;
  float humidity = 5;

  if(isnan(humidity) || isnan(temperature)) {
    delay(dht.getMinimumSamplingPeriod());


    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity, 1);
    Serial.print("\t\t");
    Serial.print(temperature, 1);
    Serial.print("\t\t");
    Serial.print(dht.toFahrenheit(temperature), 1);
    Serial.print("\t\t");
    Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
    Serial.print("\t\t");
    Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);
  
    String tempString = "";     // empty string
    tempString.concat(temperature);
    sprintf(msg, "{\"from\":\"device\",\"message\":\"Temperatura\",\"save\":true,\"value\": %s}", tempString);
    client.publish(topic_temp, msg, true); //Se actualiza el nuevo estado en el topic
  
    String humString = "";     // empty string
    humString.concat(humidity);
    sprintf(msg, "{\"from\":\"device\",\"message\":\"Humedad\",\"save\":true,\"value\": %s}", humString);
    client.publish(topic_hum, msg, true); //Se actualiza el nuevo estado en el topic

    delay(100);
  }

}