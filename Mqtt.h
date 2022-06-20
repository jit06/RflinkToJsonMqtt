#ifndef H_MQTT
#define H_MQTT

#include "Common.h"
//#include "GlobalVariables.h"

void mqttCallback(char* topic, byte* payload, unsigned int len);
void buildMqttChannel(char *name, char *ID);
boolean mqttConnect();
void mqttSendMessage(char* message);

#ifdef MQTT_SERVER_MODE_DNS
const char SERVER[] = MQTT_SERVER_NAME;
#else
IPAddress SERVER MQTT_SERVER_IP ;
#endif

#endif
