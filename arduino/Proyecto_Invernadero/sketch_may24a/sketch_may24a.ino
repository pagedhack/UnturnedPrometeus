/*
 Ejemplo del canal de Youtube "MikroTutoriales"
 https://www.youtube.com/@MikroTutoriales16
 
 Utilizando el broker MQTT de mqtt.mikrodash.com
 y la plataforma de https://app.mikrodash.com
 para crear una dashboard personalizada
*/

/**
 * Librerias de WiFi y utilidades para publicar en MQTT
*/

#include <WiFi.h>
//#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <Arduino_JSON.h> //https://github.com/arduino-libraries/Arduino_JSON

/**
 * 
 * Para la pantalla OLED
 * 
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h> //https://github.com/adafruit/Adafruit_SSD1306
#include <Adafruit_GFX.h>
#include <Fonts/FreeSerif9pt7b.h>

// Definir constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 64 // alto pantalla OLED
// Objeto de la clase Adafruit_SSD1306
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

//Para el sensor de Temperatura DHT11 y DHT22
#include "DHTesp.h" // http://librarymanager/All#DHTesp

#ifdef ESP32
#pragma message(EXAMPLE FOR ESP8266 ON MIKRODASH!)
//#error Select ESP8266 board.
#endif

DHTesp dht;

//PINES DE ENTRADA
#define PIN_TEMP 2
#define PIN_SDA 4
#define PIN_SCL 5

//Topics MQTT
const char* topic_temp = "646e39ec5c093741fd3a7efd/temperatura";
const char* topic_hum = "";
// Constantes de configuracion
const char* ssid = "Doppelnetz";              //Nombre Red Wi-Fi
const char* password = "13161912";           //Contrasenia Red Wi-Fi
const char* mqtt_server = "mqtt.mikrodash.com"; //Servidor MQTT
const int mqtt_port = 1883;                     //Puerto MQTT
const int qos_level=1;                          //Calidad del servicio MQTT
#define MSG_BUFFER_SIZE  (70)
char msg[MSG_BUFFER_SIZE];

#define MINUTES 1
#define WAIT_MIN (1000UL * 60 * MINUTES)
unsigned long rolltime = millis() + WAIT_MIN;

//Variables para la conexión y envio de mensajes MQTT
WiFiClient espClient;
PubSubClient client(espClient);

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
    clientId += String(random(0xffff), HEX);

    //Cuando se conecta
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");

      //Topics a los que se suscribira para recibir cambios
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

void setup()
{
  delay(10);
  Serial.begin(9600);      //Comunicación serial para monitorear
  setup_wifi();               //Se llama la funcición para conectarse al Wi-Fi
  client.setServer(mqtt_server, mqtt_port);  //Se indica el servidor y puerto de MQTT para conectarse
  
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);
  dht.setup(PIN_TEMP, DHTesp::DHT11); // Cambiar DHT11 por DHT22 sino obtienes datos

  Wire.begin(PIN_SDA, PIN_SCL);
  // Iniciar pantalla OLED en la dirección 0x3C/D
  //if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  //  Serial.println("No se encuentra la pantalla OLED");
  //}
}

bool first = true;
void loop()
{
  if (!client.connected()) { //Si se detecta que no hay conexión con el broker 
    reconnect();              //Reintenta la conexión
  }
  //client.loop();            //De lo contrario crea un bucle para las suscripciones MQTT

  if((long)(millis() - rolltime) >= 0 || first) {
    delay(dht.getMinimumSamplingPeriod());

    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();

    setTextOled(temperature, humidity);
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

    rolltime += WAIT_MIN;
    first = false;
  }

}

void setTextOled(float temp, float hum){
  // Limpiar buffer
  display.clearDisplay();
  // Tamaño del texto
  display.setFont(&FreeSerif9pt7b);
  //display.setTextSize(1);
  // Color del texto
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(temp);
  display.drawCircle(96, 10, 2, WHITE);
  //display.write(167);
  display.println(" C");
  display.setCursor(0, 50);
  display.print("Hume: ");
  display.print(hum);
  display.println("%");
  // Enviar a pantalla
  display.display();
}