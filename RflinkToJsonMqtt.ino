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

IPAddress IP STATIC_IP ;
byte mac[] = ETHER_MAC ;

#ifdef MQTT_SERVER_MODE_DNS
const char SERVER[] = MQTT_SERVER_NAME;
#else
IPAddress SERVER MQTT_SERVER_IP ;
#endif

// Serial iterator counter
int CPT;
// reconnection attemps timer counter
long lastReconnectAttempt = 0;

SoftwareSerial mySerial(8, 9); // RX, TX

EthernetClient ethClient;
PubSubClient client(SERVER, MQTT_PORT, callback, ethClient);


/**
 * callback to handle rflink order received from MQTT subscribtion
 */
void callback(char* topic, byte* payload, unsigned int len) {
  mySerial.write(payload, len);
  mySerial.print(F("\r\n"));
  Serial.println(F("=== Mqtt packet ==="));
  Serial.print(F("message = "));
  Serial.write(payload, len);
  Serial.print(F("\r\n"));
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
 * send formated messagee to serial
 */
void printToSerial() {
  Serial.println(F("=== rflink packet ==="));
  Serial.print(F("Raw data = "));Serial.print(BUFFER);    
  Serial.print(F("Mqtt pub = "));
  Serial.print(MQTT_CHAN);
  Serial.print("/");
  Serial.println(JSON);
  Serial.println();
}


/**
 * try to connect to MQTT Server
 */
boolean MqttConnect() {
  
  // connect to Mqtt server and subcribe to order channel
  if (client.connect(MQTT_RFLINK_CLIENT_NAME)) {
    client.subscribe(MQTT_RFLINK_ORDER_CHANNEL);
  }

  // report mqtt connection status
  Serial.print(F("Mqtt connection state : "));
  Serial.println(client.state());
  
  return client.connected();
}


/*********************************************************************************
 * Classic arduino bootstrap
/*********************************************************************************/
void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println(F("Init serial done"));

  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(57600);
  Serial.println(F("Init software serial done"));

#ifdef MQTT_SERVER_MODE_DNS
Serial.print(F("Mqtt server name set to : "));
Serial.println(SERVER);
#endif
  
  // start ethernet connection, trying DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Dhcp failed, start with static IP"));
    Ethernet.begin(mac, IP);  
  }
  
  Serial.print(F("Ethernet IP : "));
  Serial.println(Ethernet.localIP());

  MqttConnect();
}


/*********************************************************************************
 * Main loop
/*********************************************************************************/
void loop() {

  // handle lost of connection : retry after 2s on each loop
  if (!client.connected()) {
    Serial.println(F("Not connected, retrying in 2s"));
    delay(2000);
    MqttConnect();
  } else {
    // if something arrives from rflink
    if(mySerial.available()) {
      // bufferize serial message
      while(mySerial.available() && CPT < BUFFER_SIZE) {
        BUFFER[CPT] = mySerial.read();
        CPT++;
      }
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
  
    client.loop();
  }
}

