#include "Common.h"
#include "Rflink.h"

/*********************************************************************************
 * Global Variables
/*********************************************************************************/
// main input / output buffers
char BUFFER [BUFFER_SIZE];
char JSON   [BUFFER_SIZE];

// message builder buffers
char MQTT_NAME[MAX_DATA_LEN];
char MQTT_ID  [MAX_DATA_LEN];
char MQTT_CHAN[MAX_CHANNEL_LEN];
char FIELD_BUF[MAX_DATA_LEN];

//IPAddress IP STATIC_IP ;
//byte mac[] = ETHER_MAC ;

#ifdef MQTT_SERVER_MODE_DNS
const char SERVER[] = MQTT_SERVER_NAME;
#else
IPAddress SERVER MQTT_SERVER_IP ;
#endif

// Serial iterator counter
int CPT;
// reconnection attemps timer counter
long lastReconnectAttempt = 0;

SoftwareSerial mySerial(SOFTWARE_SERIAL_RX, SOFTWARE_SERIAL_TX); // RX, TX

//EthernetClient ethClient;
WiFiClient espClient;
PubSubClient client(espClient);


/**
 * callback to handle rflink order received from MQTT subscribtion
 */
void mqttCallback(char* topic, byte* payload, unsigned int len) {
  
  mySerial.write(payload, len);
  mySerial.print(F("\r\n"));
  Serial.println(F("\n=== Mqtt packet ==="));
  Serial.print(F("message => "));
  Serial.write(payload, len);
  Serial.print(F("\n"));
}

/**
 * build MQTT channel name to pubish to using parsed NAME and ID from rflink message
 */
void buildMqttChannel(char *name, char *ID) {
  MQTT_CHAN[0] = '\0';
  strcat(MQTT_CHAN,MQTT_PUBLISH_CHANNEL);
  strcat(MQTT_CHAN,"/");
  strcat(MQTT_CHAN,MQTT_NAME);
  strcat(MQTT_CHAN,"/");
  strcat(MQTT_CHAN,MQTT_ID);;
  strcat(MQTT_CHAN,"\0");
}

/**
 * send formated message to serial
 */
void printToSerial() {
  Serial.println(F("\n=== rflink packet ==="));
  Serial.print(F("raw  => ")); if(BUFFER[strlen(BUFFER)-1] == '\n') Serial.print(BUFFER); else Serial.println(BUFFER);    
  Serial.print(F("mqtt => ")); Serial.print(MQTT_CHAN); Serial.print(F("/")); Serial.println(JSON);
}

/**
 * try to connect to MQTT Server
 */
boolean mqttConnect() {
  Serial.print(F("Connecting to MQTT..."));
  client.setServer(SERVER,MQTT_PORT);
  client.setCallback(mqttCallback);

  while (!client.connected()) {
    // connect to Mqtt server and subcribe to order channel
    if (client.connect(MQTT_RFLINK_CLIENT_NAME)) {
      client.subscribe(MQTT_RFLINK_ORDER_CHANNEL);
      Serial.print(" OK");
    }
    else {
      Serial.print(F("ERROR ("));
      Serial.print(client.state());
      Serial.println(F(")"));
      Serial.println(F("retry in 5 secs"));
      delay(5000);   
     }
  }
  
  Serial.print(F(" ("));
  Serial.print(SERVER);
  Serial.println(F(")"));
  
  return client.connected();
}


/**
 * try to connect to Wifi
 */
void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi..."));

  while(WiFi.status() != WL_CONNECTED){
    delay(200);
  }
  Serial.print(F(" OK ("));
  Serial.print(WiFi.localIP());
  Serial.println(F(")"));
}


/**
 * Setup software serial to get RFlink output
 */
void initSoftwareSerial() {
  Serial.print(F("Init software serial..."));
  mySerial.begin(57600); // RF Link output at 57600
  Serial.println(F(" OK"));
}


/*********************************************************************************
 * Classic arduino bootstrap
/*********************************************************************************/
void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    Serial.print("."); // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println(F("\n\n\n======= RFLINK TO JSON MQTT - ESP8266 ======="));
  
  initSoftwareSerial();
  wifiConnect();
  mqttConnect();
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
  if(mySerial.available() > 0) {
    // bufferize serial message until we find end of mqtt message (\n)
    while(mySerial.available() > 0 && CPT < BUFFER_SIZE) {
      BUFFER[CPT] = mySerial.read();
      CPT++;
      if(BUFFER[CPT-1] == '\n') break;
    }

    // we start parsing rflink only if last read char is end of message (\n)
    if(BUFFER[CPT-1] == '\n') {
      BUFFER[CPT]='\0';
      CPT=0;
   
      // parse what we just read
      readRfLinkPacket(BUFFER);
      // construct channel name to publish to
      buildMqttChannel(MQTT_NAME, MQTT_ID);
      // publish to MQTT server
      client.publish(MQTT_CHAN,JSON);
      // report message for debugging
      printToSerial();
    }
  }

  client.loop();
}
