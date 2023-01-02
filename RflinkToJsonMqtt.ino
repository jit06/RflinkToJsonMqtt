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

#define MSG_RFLINK_PACKET   F("\n=== rflink message ===")
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
// This is used to log to "standard" USB to Serial port 
SoftwareSerial* Logger;

// Serial iterator counter
int CPT = 0;

/**
 * send formated message to serial and web loggers
 */
void printToLogger() {
  Logger->println(MSG_RFLINK_PACKET);
  webSocket.textAll(MSG_RFLINK_PACKET);webSocket.textAll(MSG_RFLINK_CR);
  
  Logger->print(MSG_RFLINK_RAW);
  webSocket.textAll(MSG_RFLINK_RAW);
  if(BUFFER[strlen(BUFFER)-1] == '\n') { 
    Logger->print(BUFFER);
    webSocket.textAll(BUFFER);
  } else {
    Logger->println(BUFFER);
    webSocket.textAll(BUFFER);webSocket.textAll(MSG_RFLINK_CR);
  }
    
  Logger->print(MSG_RFLINK_MQTT);  webSocket.textAll(MSG_RFLINK_MQTT); 
  Logger->print(MQTT_CHAN);        webSocket.textAll(MQTT_CHAN);
  Logger->print(MSG_RFLINK_SEP);   webSocket.textAll(MSG_RFLINK_SEP);
  Logger->println(JSON);           webSocket.textAll(JSON);webSocket.textAll(MSG_RFLINK_CR);
}

void doNothing() {
  Logger->println("\n\n === HALTED ===");
  while(1);
}

/**
 * try to connect to Wifi
 */
void wifiConnect() {
  controlStatusLed(STATUS_LED_WIFI, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(OTA_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Logger->print(F("Connecting to WiFi..."));
  int wifiStatus = -1;

  do {
    wifiStatus = WiFi.status();

    switch(wifiStatus) {
      case 0 : Logger->print(F(" idle"));break;
      case 1 : Logger->print(F(" no SSID available"));doNothing();
      case 4 : Logger->print(F(" connection failed"));doNothing();
      case 6 : Logger->print(F("\t wrong password"));doNothing();
      case 7 : Logger->print(F("."));break;
    }

    controlStatusLed(STATUS_LED_WIFI, HIGH);
    delay(200);
    controlStatusLed(STATUS_LED_WIFI, LOW);
    delay(200);
    
  } while(wifiStatus!= WL_CONNECTED);

  Logger->print(F(" OK ("));
  Logger->print(WiFi.localIP());
  Logger->println(F(")"));
  controlStatusLed(STATUS_LED_WIFI, HIGH);
}


/**
 * Setup OTA update
 */
void initOTA() {
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

  ArduinoOTA.onStart([]() { Logger->println(F("\n=== OTA Start ===")); controlStatusLed(STATUS_LED_OTA , HIGH); });
  ArduinoOTA.onEnd([]()   { Logger->println(F("\n=== OTA End ===")); controlStatusLed(STATUS_LED_OTA , LOW); });
  
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
    Logger->printf("Error[%u]: ", error);
    if      (error == OTA_AUTH_ERROR)     Logger->println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR)    Logger->println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR)  Logger->println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR)  Logger->println(F("Receive Failed"));
    else if (error == OTA_END_ERROR)      Logger->println(F("End Failed"));
  });
  
  ArduinoOTA.begin();
}


/*********************************************************************************
 * Classic arduino bootstrap
/*********************************************************************************/
void setup() {

  initStatusLeds();
  
  // configure hardware serial
  Serial.begin(57600);
#ifdef USE_SERIAL1_TX
  Serial1.begin(57600);
#endif
  // swap to hardware serial is now on D7/D8
  Serial.swap();

  // set software serial on native serial pins and usb
  Logger = new SoftwareSerial(3, 1); 
  Logger->begin(9600);
  
  delay(1000);
  
  // starting to speak...
  Logger->println(F("\n\n\n======= RFLINK TO JSON MQTT - ESP8266 ======="));
  Logger->println("Serial ports configured");

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
    Logger->print(F("Wifi disconnected (state = "));
    Logger->print(wifiState);
    Logger->println(F(")"));
    wifiConnect();
  }
    
  mqttState = client.state();
  // handle lost of connection
  if (mqttState != 0) {
    Logger->print(F("MQTT disconnected (state = "));
    Logger->print(mqttState);
    Logger->println(F(")"));
    mqttConnect();
  }

  // if something arrives from rflink
  if(Serial.available() > 0) {
    controlStatusLed(STATUS_LED_IN, HIGH);

    while(Serial.available() > 0 && CPT < BUFFER_SIZE -1) {
      BUFFER[CPT++] = Serial.read();

      if(BUFFER[CPT-1] == '\n') {
        BUFFER[CPT] = '\0';
        CPT=0;
        // parse what we just read
        if(readRfLinkPacket(BUFFER)) {
          mqttSendMessage(JSON);
          printToLogger();
        }
      }
    }
    controlStatusLed(STATUS_LED_IN, LOW);
  }
  
  client.loop();
  ArduinoOTA.handle();
  webSocket.cleanupClients();
}
