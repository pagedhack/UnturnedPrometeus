/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <WiFi.h>
#include <AsyncTCP.h>

// #include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

#include <ArduinoJson.h>

#define DHTpin 15

DHTesp dht;
// uint8_t BUILTIN_LED = 2;

// Update these with values suitable for your network.

// const char* ssid = "RedAnonima_2.4G_2.4Gnormal";
// const char* password = "Competitiva2";
// const char* mqtt_server = "192.168.100.50";

const char* ssid = "Doppelnetz";
const char* password = "13161912";
const char* mqtt_server = "mqtt.mikrodash.com";

//Credenciales MQTT MikroDash
const char* usernameMQTT = "esp32_1685037382188";
const char* passwordMQTT = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhY2wiOiI0IiwiZXhwIjoxNjg3NTY0NzI0LCJpYXQiOjE2ODUwMzc1MjQsInVzZXJfaWQiOiI2NDZlMzllYzVjMDkzNzQxZmQzYTdlZmQiLCJ1c2VybmFtZSI6IiJ9.xdic9C_79LPyOkTMh8WnADbgJiIjQTaVhKZ8oYPfaTs";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-Samuel";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), usernameMQTT, passwordMQTT) ) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("fei/cc1/temperatura/samuel", "temperatura");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  dht.setup(DHTpin, DHTesp::DHT11);

  pinMode(BUILTIN_LED, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(dht.getMinimumSamplingPeriod());

  float temperatura = 23.6;  //dht.getTemperature();
  float humedad = 26.7;      //dht.getHumidity();
  float latitud = 0;
  float longitud = 0;

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.print("No se pudo leer la temperatura");
    return;
  }

  // Serial.print( dht.getStatusString() );
  // Serial.print("\t");
  // Serial.print(humedad, 1);
  // Serial.print("\t\t");
  // Serial.print(temperatura, 1);
  // Serial.print("\t\t");
  // Serial.print( dht.toFahrenheit(temperatura), 1);
  // Serial.print("\t\t");
  // Serial.print( dht.computeHeatIndex(temperatura, humedad, falseu9n), 1);
  // Serial.print("\t\t");
  // Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperatura), humedad, true), 1);
  // delay(2000);

  //json

  //StaticJsonBuffer<300> JSONbuffer;
  //JsonObject& JSONencoder = JSONbuffer.createObject();

  //JSONencoder["temperatura"] = temperatura;
  //JSONencoder["humedad"] = humedad;
  //JSONencoder["latitud"] = latitud;
  //JSONencoder["longitud"] = longitud;

  //char JSONmessageBuffer[100];
  //JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);
  //if (client.publish("esp/test", JSONmessageBuffer) == true) {
  //  Serial.println("Success sending message");
  //} else {
  //  Serial.println("Error sending message");
  //}
  delay(5000);

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf(msg, MSG_BUFFER_SIZE, "En el cc1 #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("fei/cc1/temperatura/samuel",JSONmessageBuffer);
  }
}
