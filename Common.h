#ifndef H_COMMON
#define H_COMMON

#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <Ethernet.h>
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
// Fallback IP address if DHCP fail
#define STATIC_IP (192, 168, 1, 1)
// Ethernet MAC Address
#define ETHER_MAC { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED }

/*********************************************************************************
 * functions defined in scketch
/*********************************************************************************/
void callback(char* topic, byte* payload, unsigned int length);
void buildMqttChannel(char *name, char *ID);
void printToSerial();

#endif
