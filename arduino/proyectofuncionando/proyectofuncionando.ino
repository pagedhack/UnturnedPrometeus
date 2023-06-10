#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

//Para el sensor de Temperatura DHT11
#include "DHTesp.h"

#include <AsyncTCP.h>

#ifdef ESP32
#pragma message(Proyecto para esp32 con 3 sensores)
#endif

DHTesp dht;

//PINES DE ENTRADA
#define PIN_TEMP 13
#define PIN_RAD 12
#define pinEcho 14
#define pinGatillo 27

//Topics MQTT ON LINE  -----------
const char* topic_temperatura = "646e39ec5c093741fd3a7efd/temperatura";
const char* topic_humedad = "646e39ec5c093741fd3a7efd/humedad";
const char* topic_radiacion = "646e39ec5c093741fd3a7efd/radiacion";
const char* topic_medicion = "646e39ec5c093741fd3a7efd/medicion";
const char* topic_agua = "646e39ec5c093741fd3a7efd/agua";

// Constantes de configuracion
const char* ssid = "Doppelnetz";    //Nombre Red Wi-Fi
const char* password = "13161912";  //Contrasenia Red Wi-Fi

const char* mqtt_server = "mqtt.mikrodash.com";  //Servidor MQTT
const char* mqtt_server_local = "192.168.137.1";
const int mqtt_port = 1883;  //Puerto MQTT
const int qos_level = 1;     //Calidad del servicio MQTT

#define MSG_BUFFER_SIZE (80)
char msg[MSG_BUFFER_SIZE];

#define MINUTES 15
#define WAIT_MIN (1000UL * 60 * MINUTES)
unsigned long rolltime = millis() + WAIT_MIN;

//Credenciales MQTT MikroDash
const char* nombre = "nuevo";
const char* contra = "invernadero";

//Variables para la conexión y envio de mensajes MQTT
WiFiClient espClient1;
WiFiClient espClient2;

PubSubClient client(espClient1);
PubSubClient clientLocal(espClient2);

//Variables para sensor de radiación UV
float VoltajeUV;
float ValorSensor;

void setup() {
  Serial.begin(9600);
  dht.setup(PIN_TEMP, DHTesp::DHT11);  // Cambiar DHT11 por DHT22 sino obtienes datos
  pinMode(PIN_RAD, OUTPUT);            //entrada para sensor de radiacion    
  pinMode(pinEcho, INPUT);             //entradas para el sensor de agua
  pinMode(pinGatillo, OUTPUT);

  setup_wifi();  //Se llama la funcición para conectarse al Wi-Fi
  client.setServer(mqtt_server, mqtt_port);             //Se indica el servidor y puerto de MQTT para conectarse externo
  clientLocal.setServer(mqtt_server_local, mqtt_port);  ////Se indica el servidor y puerto de MQTT para conectarse local

  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);
}

//bool first = true;
void loop() {
  ValorSensor = random(300,400);                       //Aquí se le asigna a la variable el valor obtenido del sensor
  VoltajeUV = ValorSensor / 1024 * 5.0;     //Prácticamente usa la misma estructura que un LM35 que es un sensor de temperatura.
  Serial.print("Voltaje del sensor UV: ");  //Solo se imprime el texto de referencia
  Serial.print(VoltajeUV);                  //Ya se imprime el valor obtenido en voltaje (Si quieren que sea en nanometros díganle a Mai y les paso la adaptación)
  Serial.print(" V");                       //igual decoración para indicar la unidad de medida

  //local
  if (!clientLocal.connected()) {
    reconnectLocal();  //Reintenta la conexión con el clente local
  }
  clientLocal.loop();  //De lo contrario crea un bucle para las suscripciones MQTT

  //externo
  // if (!client.connected()) {  //Si se detecta que no hay conexión con el broker
  //   reconnectExterno();       //Reintenta la conexión con el clente externo
  // }
  // client.loop();

  delay(dht.getMinimumSamplingPeriod());

  float temperatura = dht.getTemperature();
  float humedad = dht.getHumidity();

  long t;
  long distancia;
  digitalWrite(pinGatillo, LOW);
  delayMicroseconds(4);
  digitalWrite(pinGatillo, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinGatillo, LOW);
  t = pulseIn(pinEcho, HIGH);
  distancia = t / 59;

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.print("Error en los sensores");
    return;
  }
  Serial.print(dht.getStatusString());
  Serial.print("\t(\t");
  Serial.print(humedad, 1);
  Serial.print("\t\t");
  Serial.print(temperatura, 1);
  Serial.print("\t\t");
  Serial.print(distancia);
  Serial.print("\t\t");
  Serial.print(" Value = ");
  Serial.print(ValorSensor);
  Serial.print(" Voltage= ");
  Serial.print(VoltajeUV);
  Serial.println(" V");
  Serial.println();

  String tempString = "";  // empty string
  tempString.concat(temperatura);
  sprintf(msg, "{\"from\":\"device\",\"message\":\"Temperatura\",\"save\":true,\"value\": %s}", tempString);
  client.publish(topic_temperatura, msg, true);  //Se actualiza el nuevo estado en el topic

  String humString = "";  // empty string
  humString.concat(humedad);
  sprintf(msg, "{\"from\":\"device\",\"message\":\"Humedad\",\"save\":true,\"value\": %s}", humString);
  client.publish(topic_humedad, msg, true);  //Se actualiza el nuevo estado en el topic

  String radString = "";  // empty string
  radString.concat(ValorSensor);
  sprintf(msg, "{\"from\":\"device\",\"message\":\"Radiacion\",\"save\":true,\"value\": %s}", radString);
  client.publish(topic_radiacion, msg, true);  //Se actualiza el nuevo estado en el topic

  String volString = "";  // empty string
  volString.concat(VoltajeUV);
  sprintf(msg, "{\"from\":\"device\",\"message\":\"Voltaje\",\"save\":true,\"value\": %s}", volString);
  client.publish(topic_medicion, msg, true);  //Se actualiza el nuevo estado en el topic

  String disString = "";  // empty string
  disString.concat(distancia);
  sprintf(msg, "{\"from\":\"device\",\"message\":\"Distancia\",\"save\":true,\"value\": %s}", disString);
  client.publish(topic_agua, msg, true);  //Se actualiza el nuevo estado en el topic

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["temperatura"] = temperatura;
  JSONencoder["humedad"] = humedad;
  JSONencoder["voltajeradiacion"] = ValorSensor;
  JSONencoder["valorradiacion"] = VoltajeUV;
  JSONencoder["agua"] = distancia;

  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  clientLocal.publish("/temperatura", JSONmessageBuffer, true);

  delay(5000);
}

//Conexion Wi-Fi
void setup_wifi() {
  delay(1000);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  //randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}
// Funcion para reconectar al cliente local
void reconnectLocal() {
  while (!clientLocal.connected()) {
    Serial.print("Conectando al broker MQTT Local...");
    String clientIdLocal = "ESP8266Client-Invernadero";
    if (clientLocal.connect(clientIdLocal.c_str())) {
      Serial.println("connected Local");
      clientLocal.publish("/temperatura", "temperatura");
      clientLocal.subscribe("inTopic");
    } else {
      Serial.print("failed_Local, rc=");
      Serial.print(clientLocal.state());
      Serial.println(" try again in 5 seconds");
      delay(8000);
    }
  }
}
//funcion para reconectar a mickrodash
void reconnectExterno() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT Externo...");
    String clientId = "MikroDashWiFiClient";
    if (client.connect(clientId.c_str(), nombre, contra)) {
      Serial.println("Conectado Externo");
    } else {
      Serial.print("Error al conectar: ");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(8000);
    }
  }
}