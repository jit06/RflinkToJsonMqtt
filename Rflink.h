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
#define RFLINK_VALUE_TYPE_FLOAT   2
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
const char RFLINK_FIELD_NAME_BARO[]        = "BARO";
const char RFLINK_FIELD_NAME_UV[]          = "UV";
const char RFLINK_FIELD_NAME_LUX[]         = "LUX";
const char RFLINK_FIELD_NAME_RAIN[]        = "RAIN";
const char RFLINK_FIELD_NAME_RAINRATE[]    = "RAINRATE";
const char RFLINK_FIELD_NAME_WINSP[]       = "WINSP";
const char RFLINK_FIELD_NAME_AWINSP[]      = "AWINSP";
const char RFLINK_FIELD_NAME_WINGS[]       = "WINGS";
const char RFLINK_FIELD_NAME_WINDIR[]      = "WINDIR";
const char RFLINK_FIELD_NAME_WDIR[]        = "WDIR";
const char* const RFLINK_FIELD_HEXINT[] = {
  RFLINK_FIELD_NAME_WATT,
  RFLINK_FIELD_NAME_KWATT,
  RFLINK_FIELD_NAME_BARO,
  RFLINK_FIELD_NAME_UV,
  RFLINK_FIELD_NAME_LUX,
  RFLINK_FIELD_NAME_RAIN,
  RFLINK_FIELD_NAME_RAINRATE,
  RFLINK_FIELD_NAME_WINSP,
  RFLINK_FIELD_NAME_AWINSP,
  RFLINK_FIELD_NAME_WINGS,
  RFLINK_FIELD_NAME_WINDIR,
  RFLINK_FIELD_NAME_WDIR,
  "\0" // do not remove this mark the end of the array
};

// list of fields with hex encoded float value that must be converted to decimal float
const char RFLINK_FIELD_NAME_TEMP[]   = "TEMP";
const char RFLINK_FIELD_NAME_WINCHL[] = "WINCHL";
const char RFLINK_FIELD_NAME_WINTMP[] = "WINTMP";
const char* const RFLINK_FIELD_HEXFLT[] = {
  RFLINK_FIELD_NAME_TEMP,
  RFLINK_FIELD_NAME_WINCHL,
  RFLINK_FIELD_NAME_WINTMP,
  "\0" // do not remove this mark the end of the array
};

// special values that are just ignored, thus not sent to MQTT gateway (spaces must be converted to _)
const char RFLINK_MQTT_NAME_DEBUG[]      = "DEBUG";
const char RFLINK_MQTT_NAME_Debug[]      = "Debug";
const char RFLINK_MQTT_NAME_OK[]         = "OK";
const char RFLINK_MQTT_NAME_CMD_UNKNOWN[]= "CMD_UNKNOWN";
const char RFLINK_MQTT_NAME_PONG[]       = "PONG";
const char RFLINK_MQTT_NAME_PULLUP[]     = "Internal_Pullup_on_RF-in_disabled";
const char RFLINK_MQTT_NAME_NODO46[]     = "Nodo_RadioFrequencyLink_-_RFLink_Gateway_V1.1_-_R46";
const char RFLINK_MQTT_NAME_NODO47[]     = "Nodo_RadioFrequencyLink_-_RFLink_Gateway_V1.1_-_R47";
const char RFLINK_MQTT_NAME_NODO48[]     = "Nodo_RadioFrequencyLink_-_RFLink_Gateway_V1.1_-_R48";
const char RFLINK_MQTT_NAME_NODO50[]     = "Nodo_RadioFrequencyLink_-_RFLink_Gateway_V1.1_-_R50";
const char RFLINK_MQTT_NAME_NODO51[]     = "Nodo_RadioFrequencyLink_-_RFLink_Gateway_V1.1_-_R51";
const char* const  RFLINK_MQTT_NAMES_NO_JSON[] = {
  RFLINK_MQTT_NAME_DEBUG,
  RFLINK_MQTT_NAME_Debug,
  RFLINK_MQTT_NAME_OK,
  RFLINK_MQTT_NAME_CMD_UNKNOWN,
  RFLINK_MQTT_NAME_PONG,
  RFLINK_MQTT_NAME_PULLUP,
  RFLINK_MQTT_NAME_NODO46,
  RFLINK_MQTT_NAME_NODO47,
  RFLINK_MQTT_NAME_NODO48,
  RFLINK_MQTT_NAME_NODO50,
  RFLINK_MQTT_NAME_NODO51,
  "\0" // do not remove this mark the end of the array
};


/*********************************************************************************
 * RFLink functions
/*********************************************************************************/
int  readRfLinkPacket(char* line);
void readRfLinkFields(char* fields, int start);

bool RfLinkFieldIsString(char *buffer);
bool RfLinkFieldIsHexFloat(char *buffer);
bool RfLinkFieldIsHexInteger(char *buffer);

void RfLinkFieldAddQuotedValue (char *buffer);
void RfLinkFieldAddFloatValue (char *buffer);
void RfLinkFieldAddIntegerValue(char *buffer);

bool RfLinkIsStringInArray(char *buffer, const char* const strArray[]);
#endif
