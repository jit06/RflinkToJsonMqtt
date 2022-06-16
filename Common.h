#ifndef H_COMMON
#define H_COMMON

#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SPI.h>

/*********************************************************************************
 * Global parameters
/*********************************************************************************/
// Maximum size of one serial line and json content
#define BUFFER_SIZE 200
// Maximum size for record name, record id, json field name and json field value
#define MAX_DATA_LEN 24 
// Maximum channel path size (at least lenght of MQTT_PUBLISH_CHANNEL + 2 x MAX_DATA_LEN)
#define MAX_CHANNEL_LEN 60
// default MQTT server port
#define MQTT_PORT 1883
// default MQTT Channel to publish to
#define MQTT_PUBLISH_CHANNEL "rflink"
// Client name sent to MQTT server
#define MQTT_RFLINK_CLIENT_NAME "rflinkClient"
// MQTT channel to listen to for rflink order
#define MQTT_RFLINK_ORDER_CHANNEL "rflink/Order"
// MQTT Server IP
#define MQTT_SERVER_IP (192, 168, 1, 150)
// MQTT Server name
#define MQTT_SERVER_NAME "mqtt.local.lan"
// MQTT Server mode : comment out to use MQTT_SERVER_IP instead of MQTT_SERVER_NAME
#define MQTT_SERVER_MODE_DNS
// WIFI Settings
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
// software serial pin
#define SOFTWARE_SERIAL_RX 4
#define SOFTWARE_SERIAL_TX 5
// status leds (D0=16, D5=14, D6=12, D7=13, D8=15 )
#define STATUS_LED_WIFI 16
#define STATUS_LED_MQTT 14
#define STATUS_LED_IN   12
#define STATUS_LED_OUT  13
#define STATUS_LED_OTA  15
#define STATUS_LED_WEB  A0

// HTTP Server port
#define HTTP_PORT 80
// OTA settings
#define OTA_PORT 8266
#define OTA_HOSTNAME "rflink-ESP8266"
#define OTA_PASSWORD "123"

/*********************************************************************************
 * functions defined in scketch
/*********************************************************************************/
void callback(char* topic, byte* payload, unsigned int length);
void buildMqttChannel(char *name, char *ID);
void printToSerial();
void wifiConnect();
boolean mqttConnect();
void initSoftwareSerial();
void initStatusLeds();
void controlStatusLed(int pin, int state);

#endif
