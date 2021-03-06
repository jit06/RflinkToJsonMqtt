/*********************************************************************************
 * RFLink parser functions
/*********************************************************************************/
#include "Rflink.h"

/**
 * Read RFLink packet (line terminated by \n) and
 * - extract message name / driver (eg: OregonV1) to MQTT_NAME
 * - extract ID of the sender (eg: ID=012345) to MQTT_ID
 * - create a JSON message with all encountered fields
 */
void readRfLinkPacket(char* line) {
  int i = 6; // ignore message type and line number
  int j = 0;
  bool nameHasEq = false;
  
  // check len and ignore bad packet (broken serial, etc.)
  if(strlen(line) < RFLINK_PACKET_MIN_SIZE) return;

  
  // get name : 3rd field (begins at char 6)
  while(line[i] != ';' && i < BUFFER_SIZE && j < MAX_DATA_LEN) {
    if      (line[i]==' ')  MQTT_NAME[j] = '_';
    else if (line[i]=='=')  { nameHasEq = true; break; }
    else                    MQTT_NAME[j] = line[i];
    i++; j++;
  }

  // ends string correctly
  MQTT_NAME[j] = '\0';
 
  
  // if name contains "=", assumes that it's an rflink message, not an RF packet
  // thus we put a special name and ID=0, then parse to JSON
  if(nameHasEq==true) {
    Serial.println(F("name contains '=' !"));
    i = 6;
    strcpy_P(MQTT_NAME,PSTR("message"));
    MQTT_ID[0]='0'; MQTT_ID[1]='\0';
    readRfLinkFields(line, i);
    return;
  }
  
  
  // for debug and ACK messages, send them directly, no json convertion
  if(RfLinkIsStringInArray(MQTT_NAME,RFLINK_MQTT_NAMES_NO_JSON)) {
    Serial.println(F("Special name found => no JSON convertion"));
    MQTT_ID[0]='0'; MQTT_ID[1]='\0';
    j=0;
    while(line[i] != '\n' && i < BUFFER_SIZE && j < BUFFER_SIZE) {
      JSON[j++] = line[i++];
    }
    JSON[j-1]='\0';
    return;
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
      else if(RfLinkFieldIsOregon(FIELD_BUF))     valueType=RFLINK_VALUE_TYPE_OREGON;
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
        case RFLINK_VALUE_TYPE_OREGON:  RfLinkFieldAddOregonValue(FIELD_BUF); break;
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
 * check wether a given string is in a PROGMEN array of strings
 */
bool RfLinkIsStringInArray(char *buffer, char* strArray[]) {
  int i = 0;
  
  while(pgm_read_word(strArray + i) != '\0') {
    if((strcmp_P(buffer, pgm_read_word(strArray + i++))==0)) return true;
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
 * check if a given field name is used for Oregon temperature (thus need to be converted to float)
 */
bool RfLinkFieldIsOregon(char *buffer) {
   return ((strncmp_P(MQTT_NAME,PSTR("Oregon"),6)) == 0) && 
          ( strcmp_P (FIELD_BUF,PSTR("TEMP")     ) == 0);
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
void RfLinkFieldAddOregonValue(char *buffer) {
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

