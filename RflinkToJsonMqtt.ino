#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <PubSubClient.h>

#include "Common.h"
#include "Rflink.h"
#include "Webserver.h"
#include "Mqtt.h"
#include "StatusLeds.h"

#define MSG_RFLINK_PACKET   F("\n=== rflink packet ===")
#define MSG_RFLINK_RAW      F("raw  => ")
#define MSG_RFLINK_MQTT     F("mqtt => ")
#define MSG_RFLINK_SEP      F("/")
#define MSG_RFLINK_CR       F("\n")

/*
 * external global variables
 */
// from Rflink.cpp
extern char BUFFER [];
extern char JSON   [];

// from Mqtt.cpp
extern char MQTT_CHAN[];
extern PubSubClient client;

// from Webserver.h
extern AsyncWebSocket webSocket;

/*
 * local global variables
 */
// serial connection to rflink
SoftwareSerial softSerial (SOFTWARE_SERIAL_RX, SOFTWARE_SERIAL_TX); // RX, TX

// Serial iterator counter
int CPT;


/**
 * send formated message to serial
 */
void printToSerial() {
  Serial.println(MSG_RFLINK_PACKET);
  webSocket.textAll(MSG_RFLINK_PACKET);webSocket.textAll(MSG_RFLINK_CR);
  
  Serial.print(MSG_RFLINK_RAW);
  webSocket.textAll(MSG_RFLINK_RAW);
  if(BUFFER[strlen(BUFFER)-1] == '\n') { 
    Serial.print(BUFFER);
    webSocket.textAll(BUFFER);
  } else {
    Serial.println(BUFFER);
    webSocket.textAll(BUFFER);webSocket.textAll(MSG_RFLINK_CR);
  }
    
  Serial.print(MSG_RFLINK_MQTT);  webSocket.textAll(MSG_RFLINK_MQTT); 
  Serial.print(MQTT_CHAN);        webSocket.textAll(MQTT_CHAN);
  Serial.print(MSG_RFLINK_SEP);   webSocket.textAll(MSG_RFLINK_SEP);
  Serial.println(JSON);           webSocket.textAll(JSON);webSocket.textAll(MSG_RFLINK_CR);
}


/**
 * try to connect to Wifi
 */
void wifiConnect() {
  controlStatusLed(STATUS_LED_WIFI, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi..."));

  while(WiFi.status() != WL_CONNECTED){
    controlStatusLed(STATUS_LED_WIFI, HIGH);
    delay(200);
    controlStatusLed(STATUS_LED_WIFI, LOW);
  }
  Serial.print(F(" OK ("));
  Serial.print(WiFi.localIP());
  Serial.println(F(")"));
  controlStatusLed(STATUS_LED_WIFI, HIGH);
}


/**
 * Setup software serial to get RFlink output
 */
void initSoftwareSerial() {
  
  Serial.print(F("Init software serial..."));
  softSerial.begin(57600); // RF Link output at 57600
  Serial.println(F(" OK"));
}


/**
 * Setup OTA update
 */
void initOTA() {
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

  ArduinoOTA.onStart([]() { Serial.println(F("\n=== OTA Start ===")); controlStatusLed(STATUS_LED_OTA , HIGH); });
  ArduinoOTA.onEnd([]()   { Serial.println(F("\n=== OTA End ===")); controlStatusLed(STATUS_LED_OTA , LOW); });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    unsigned int percent = (progress / (total / 100));
    static int ledstate = LOW;
    
    Serial.printf("\tOTA Progress: %u%%\r", percent);
    if(percent%2 == 0) {
      ledstate == LOW ? ledstate=HIGH:ledstate=LOW;
      controlStatusLed(STATUS_LED_OTA, ledstate);
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });
  
  ArduinoOTA.begin();
}


/*********************************************************************************
 * Classic arduino bootstrap
/*********************************************************************************/
void setup() {

  initStatusLeds();
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect.
  }
  
  delay(1000);
  
  Serial.println(F("\n\n\n======= RFLINK TO JSON MQTT - ESP8266 ======="));
  
  initSoftwareSerial();
  wifiConnect();
  initOTA();
  mqttConnect();
  initWebServer();
}


/*********************************************************************************
 * Main loop
/*********************************************************************************/
void loop() {
  int mqttState=-99, wifiState=-99;

  wifiState = WiFi.status();
  if(wifiState != WL_CONNECTED) {
    Serial.print(F("Wifi disconnected (state = "));
    Serial.print(wifiState);
    Serial.println(F(")"));
    wifiConnect();
  }
    
  mqttState = client.state();
  // handle lost of connection
  if (mqttState != 0) {
    Serial.print(F("MQTT disconnected (state = "));
    Serial.print(mqttState);
    Serial.println(F(")"));
    mqttConnect();
  }

  // if something arrives from rflink
  if(softSerial.available() > 0) {
    controlStatusLed(STATUS_LED_IN, HIGH);
    // bufferize serial message until we find end of mqtt message (\n)
    while(softSerial.available() > 0 && CPT < BUFFER_SIZE) {
      BUFFER[CPT] = softSerial.read();
      CPT++;
      if(BUFFER[CPT-1] == '\n') break;
    }
    
    // we start parsing rflink only if last read char is end of message (\n)
    if(BUFFER[CPT-1] == '\n') {
      BUFFER[CPT]='\0';
      CPT=0;
   
      // parse what we just read
      readRfLinkPacket(BUFFER);
      mqttSendMessage(JSON);
      // report message for debugging
      printToSerial();
      controlStatusLed(STATUS_LED_IN, LOW);
    }
  }
  
  client.loop();
  ArduinoOTA.handle();
  webSocket.cleanupClients();
}
