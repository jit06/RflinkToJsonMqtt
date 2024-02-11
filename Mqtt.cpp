#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "Webserver.h"

#include "Mqtt.h"
#include "Common.h"
#include "StatusLeds.h"

#define MSG_MQTT_PACKET   F("== MQTT ==")
#define MSG_MQTT_MSG      F("msg << ")
#define MSG_MQTT_CR       F("\n")

/*
 * external global variables
 */
// from Webserver.cpp
extern AsyncWebSocket webSocket;
// from main sketch file
extern SoftwareSerial* Logger;

/*
 * local global variables
 */
// message builder buffers
char MQTT_NAME[MAX_DATA_LEN];
char MQTT_ID  [MAX_DATA_LEN];
char MQTT_CHAN[MAX_CHANNEL_LEN];

// mqtt client
WiFiClient espClient;
PubSubClient client(espClient);


/**
 * callback to handle rflink order received from MQTT subscribtion
 */
void mqttCallback(char* topic, byte* payload, unsigned int len) {
  controlStatusLed(STATUS_LED_OUT, HIGH);

#ifdef USE_SERIAL1_TX
  Serial1.write(payload, len);
  Serial1.print(F("\r\n"));
#else
  Serial.write(payload, len);
  Serial.print(F("\r\n"));
#endif

  Logger->println(MSG_MQTT_PACKET);  webSocket.textAll(MSG_MQTT_PACKET);webSocket.textAll(MSG_MQTT_CR);
  Logger->print(MSG_MQTT_MSG);       webSocket.textAll(MSG_MQTT_MSG);
  Logger->write(payload, len);       webSocket.textAll(payload, len);
  Logger->print(MSG_MQTT_CR);        webSocket.textAll(MSG_MQTT_CR);
  
  controlStatusLed(STATUS_LED_OUT, LOW);
}


/**
 * send message to mqtt server
 */
void mqttSendMessage(char* message) {
  // construct channel name to publish to
  buildMqttChannel(MQTT_NAME, MQTT_ID);
  // publish to MQTT server
  client.publish(MQTT_CHAN,message);
}


/**
 * build MQTT channel name to publish to using parsed NAME and ID from rflink message
 */
void buildMqttChannel(char *name, char *ID) {
  MQTT_CHAN[0]='\0';
  strcat(MQTT_CHAN,MQTT_PUBLISH_CHANNEL);
  strcat(MQTT_CHAN,"/");
  strcat(MQTT_CHAN,MQTT_NAME);
  strcat(MQTT_CHAN,"/");
  strcat(MQTT_CHAN,MQTT_ID);;
  strcat(MQTT_CHAN,"\0");
}


/**
 * try to connect to MQTT Server
 */
boolean mqttConnect() {
  controlStatusLed(STATUS_LED_MQTT, LOW);
  Logger->print(F("Connecting to MQTT..."));
  client.setServer(SERVER,MQTT_PORT);
  client.setCallback(mqttCallback);

  while (!client.connected()) {
    // connect to Mqtt server and subcribe to order channel
    if (client.connect(MQTT_RFLINK_CLIENT_NAME)) {
      client.subscribe(MQTT_RFLINK_ORDER_CHANNEL);
      controlStatusLed(STATUS_LED_MQTT, HIGH);
      Logger->print(" OK");
    }
    else {
      controlStatusLed(STATUS_LED_MQTT, HIGH);
      delay(1000);
      controlStatusLed(STATUS_LED_MQTT, LOW);
      Logger->print(F("ERROR ("));
      Logger->print(client.state());
      Logger->println(F(")"));
      Logger->println(F("retry in 5 secs"));
      delay(4000);
    }
  }

  Logger->print(F(" ("));
  Logger->print(SERVER);
  Logger->println(F(")"));
  controlStatusLed(STATUS_LED_MQTT, HIGH);

  return client.connected();
}
