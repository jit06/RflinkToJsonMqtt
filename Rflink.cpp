/*********************************************************************************
 * RFLink parser functions
/*********************************************************************************/
#include <SoftwareSerial.h>
#include "Rflink.h"

// main input / output buffers
char BUFFER [BUFFER_SIZE];
char JSON   [BUFFER_SIZE];
char FIELD_BUF[MAX_DATA_LEN];

extern char MQTT_NAME[];
extern char MQTT_ID[];

// from main sketch file
extern SoftwareSerial* Logger;

/**
 * Read RFLink packet (line terminated by \n) and
 * - extract message name / driver (eg: OregonV1) to MQTT_NAME
 * - extract ID of the sender (eg: ID=012345) to MQTT_ID
 * - create a JSON message with all encountered fields
 */
int readRfLinkPacket(char* line) {
  int i = 6; // ignore message type and line number (xx;yy;)
  int j = 0;
  bool nameHasEq = false;

  // check len and ignore bad packet (broken serial, etc.)
  if( strlen(line) < RFLINK_PACKET_MIN_SIZE || // consider that very small string are not good
      line[i-1] != ';'                         // if the 5th char is not ';', something went wrong and the line is not well formed
    ) {
    Logger->print(F("*Not an RFlink payload: "));
    Logger->print(line);
    return 0;
  }

  // get name : 3rd field (begins at char 6). Spaces and slashes are replaced by underscore
  while(line[i] != ';' && line[i] != '\r' && i < BUFFER_SIZE && j < MAX_DATA_LEN) {
    if      (line[i]==' ')  MQTT_NAME[j] = '_';
    else if (line[i]=='/')  MQTT_NAME[j] = '_';
    else if (line[i]=='=')  { nameHasEq = true; break; }
    else                    MQTT_NAME[j] = line[i];
    i++; j++;
  }

  // ends string correctly
  MQTT_NAME[j] = '\0';

  // if name contains "=", assumes that it's an rflink message, not an RF packet
  // thus we put a special name and ID=0, then parse to JSON
  if(nameHasEq==true) {
    Logger->println(F("*rflink message detected (name contains '=')"));
    i = 6;
    strcpy_P(MQTT_NAME,PSTR("message"));
    MQTT_ID[0]='0'; MQTT_ID[1]='\0';
    readRfLinkFields(line, i);
    return 1;
  }


  // for debug and ACK messages just ignore them
  if(RfLinkIsStringInArray(MQTT_NAME,RFLINK_MQTT_NAMES_NO_JSON)) {
    Logger->print(F("*Ignored message: "));
    Logger->print(line);
    return 0;
  }


  // for all other messages, get MQTT_ID (4th field) then convert to json
  j=0;
  i+=4; // skip ";MQTT_ID="
  while(line[i] != ';' && i < BUFFER_SIZE && j < MAX_DATA_LEN) {
    MQTT_ID[j++] = line[i++];
  }
  MQTT_ID[j] = '\0';
  // continue with json convertion
  readRfLinkFields(line, i+1);

  return 1;
}


/**
 * Extract fields and convert them to a json string
 */
void readRfLinkFields(char* fields, int start){

  int strpos=start;
  int fldpos=0;
  int valueType=0;

  JSON[0]='{';
  JSON[1]='\0';

  while(strpos < BUFFER_SIZE-start && fields[strpos] != '\n' && fields[strpos] != '\0') {

    // if current char is "=", we end name parsing and start parsing the field's value
    if(fields[strpos] == '=') {
      FIELD_BUF[fldpos]='\0';
      fldpos=0;

      // Tag field regarding the name...
      if(RfLinkFieldIsString(FIELD_BUF))          valueType=RFLINK_VALUE_TYPE_STRING;
      else if(RfLinkFieldIsHexFloat(FIELD_BUF))   valueType=RFLINK_VALUE_TYPE_FLOAT;
      else if(RfLinkFieldIsHexInteger(FIELD_BUF)) valueType=RFLINK_VALUE_TYPE_INTEGER;
      else                                        valueType=RFLINK_VALUE_TYPE_RAWVAL;

      RfLinkFieldAddQuotedValue(FIELD_BUF);

    // if current char is ";", we end parsing value and start parsing another field's name
    } else if(fields[strpos] == ';') {

      FIELD_BUF[fldpos]='\0';
      fldpos=0;
      strcat(JSON,":");

      // Handle special cases...
      switch(valueType) {
        case RFLINK_VALUE_TYPE_STRING:  RfLinkFieldAddQuotedValue(FIELD_BUF); break;
        case RFLINK_VALUE_TYPE_FLOAT:   RfLinkFieldAddFloatValue(FIELD_BUF); break;
        case RFLINK_VALUE_TYPE_INTEGER: RfLinkFieldAddIntegerValue(FIELD_BUF);break;
        default : strcat(JSON,FIELD_BUF);
      }

      strcat(JSON,",");
    } else { // default case : copy current char
      FIELD_BUF[fldpos++]=fields[strpos];
    }

    strpos++;
  }

  int len = strlen(JSON);
  JSON[len-1]='}';
}


/**
 * check wether a given string is in an array of strings
 */
bool RfLinkIsStringInArray(char *buffer, const char* const strArray[]) {
  int i = 0;

  while(strArray[i][0] != '\0') {
    if((strcmp(buffer, (strArray[i++]))==0)) return true;
  }

  return false;
}


/**
 * check if a given field name is used for string (thus need quotes in JSON message)
 */
bool RfLinkFieldIsString(char *buffer) {
  return RfLinkIsStringInArray(buffer, RFLINK_FIELD_STRING);
}


/**
 * check if a given field name is used for hex integer (thus need to be converted to dec)
 */
bool RfLinkFieldIsHexInteger(char *buffer) {
  return RfLinkIsStringInArray(buffer, RFLINK_FIELD_HEXINT);
}


/**
 * check if a given field name is used for hex float (thus need to be converted to dec)
 */
bool RfLinkFieldIsHexFloat(char *buffer) {
  return RfLinkIsStringInArray(buffer, RFLINK_FIELD_HEXFLT);
}


/**
 * put the value as a quoted one in the JSON buffer
 */
void RfLinkFieldAddQuotedValue(char *buffer) {
  strcat(JSON,"\"");
  strcat(JSON,buffer);
  strcat(JSON,"\"");
}


/**
 * convert a string to float value and put it in the JSON buffer
 * eg : 0x00c3 which is 216 (dec) will become 21.6
 */
void RfLinkFieldAddFloatValue(char *buffer) {
  char strfloat[4];
  dtostrf(strtoul(buffer,NULL,16)*0.1, 2, 1, strfloat);
  strfloat[4]='\0';
  strcat(JSON,strfloat);
}


/**
 * put the value as an integer in the JSON buffer
 * eg: 0x57 will become 87
 */
void RfLinkFieldAddIntegerValue(char *buffer) {
  char s[21];
  strcat(JSON, ultoa(strtoul(buffer,NULL,16),s,10));
}
