#ifndef H_RFLINK
#define H_RFLINK

#include <Arduino.h>
#include "Common.h"

/*********************************************************************************
 * RFLink settings
/*********************************************************************************/
// Rflink mqtt channel name
#define RFLINK_MQTT_CHANNEL "rflink"

// if less than that, rflink line will be ignored
#define RFLINK_PACKET_MIN_SIZE 8  

// Rflink value type tags
#define RFLINK_VALUE_TYPE_INTEGER 0
#define RFLINK_VALUE_TYPE_STRING  1
#define RFLINK_VALUE_TYPE_OREGON  2
#define RFLINK_VALUE_TYPE_RAWVAL  3


// main input / output buffers
//extern char BUFFER [BUFFER_SIZE];
//extern char JSON   [BUFFER_SIZE];

// message builder buffers
//extern char MQTT_NAME[MAX_DATA_LEN];
//extern char MQTT_ID  [MAX_DATA_LEN];
//extern char FIELD_BUF[MAX_DATA_LEN];


/*********************************************************************************
 * const strings used in helper functions
/*********************************************************************************/
// list of fields that must be quoted in JSON convertion
const char RFLINK_FIELD_NAME_CMD[]         = "CMD";
const char RFLINK_FIELD_NAME_BAT[]         = "BAT";
const char RFLINK_FIELD_NAME_SMOKEALERT[]  = "SMOKEALERT";
const char RFLINK_FIELD_NAME_SWITCH[]      = "SWITCH";
const char RFLINK_FIELD_NAME_PIR[]         = "PIR";
const char RFLINK_FIELD_NAME_RFDEBUG[]     = "RFDEBUG";
const char* const RFLINK_FIELD_STRING[] = {
  RFLINK_FIELD_NAME_CMD,
  RFLINK_FIELD_NAME_BAT,
  RFLINK_FIELD_NAME_SMOKEALERT,
  RFLINK_FIELD_NAME_SWITCH,
  RFLINK_FIELD_NAME_PIR,
  RFLINK_FIELD_NAME_RFDEBUG,
  "\0" // do not remove this mark the end of the array
};


// list of fields with hex encoded integer that must be converted to decimal integer
const char RFLINK_FIELD_NAME_WATT[]        = "WATT";
const char RFLINK_FIELD_NAME_KWATT[]       = "KWATT";
const char* const RFLINK_FIELD_HEXINT[] = {
  RFLINK_FIELD_NAME_WATT,
  RFLINK_FIELD_NAME_KWATT,
  "\0" // do not remove this mark the end of the array
};


// list of detected MQTT names that implies no json convertion, but a direct copy of the buffer
const char RFLINK_MQTT_NAME_DEBUG[]      = "DEBUG";
const char RFLINK_MQTT_NAME_Debug[]      = "Debug";
const char RFLINK_MQTT_NAME_OK[]         = "OK";
const char RFLINK_MQTT_NAME_CMD_UNKNOWN[]= "CMD_UNKNOWN";
const char RFLINK_MQTT_NAME_PONG[]       = "PONG";
const char RFLINK_MQTT_NAME_NODO[]       = "Nodo RadioFrequencyLink";
const char* const  RFLINK_MQTT_NAMES_NO_JSON[] = {
  RFLINK_MQTT_NAME_DEBUG,
  RFLINK_MQTT_NAME_Debug,
  RFLINK_MQTT_NAME_OK,
  RFLINK_MQTT_NAME_CMD_UNKNOWN,
  RFLINK_MQTT_NAME_PONG,
  RFLINK_MQTT_NAME_NODO,
  "\0" // do not remove this mark the end of the array
};


/*********************************************************************************
 * RFLink functions
/*********************************************************************************/
int  readRfLinkPacket(char* line);
void readRfLinkFields(char* fields, int start);

bool RfLinkFieldIsString(char *buffer);
bool RfLinkFieldIsOregon(char *buffer);
bool RfLinkFieldIsHexInteger(char *buffer);

void RfLinkFieldAddQuotedValue (char *buffer);
void RfLinkFieldAddOregonValue (char *buffer);
void RfLinkFieldAddIntegerValue(char *buffer);

bool RfLinkIsStringInArray(char *buffer, const char* const strArray[]);
#endif
