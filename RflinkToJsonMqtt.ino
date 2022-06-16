#include "Common.h"
#include "Rflink.h"
#include "Webserver.h"
#include <ArduinoOTA.h>

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
  controlStatusLed(STATUS_LED_OUT, HIGH);
  mySerial.write(payload, len);
  mySerial.print(F("\r\n"));
  Serial.println(F("\n=== Mqtt packet ==="));
  Serial.print(F("message => "));
  Serial.write(payload, len);
  Serial.print(F("\n"));
  controlStatusLed(STATUS_LED_OUT, LOW);
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
  controlStatusLed(STATUS_LED_MQTT, LOW);
  Serial.print(F("Connecting to MQTT..."));
  client.setServer(SERVER,MQTT_PORT);
  client.setCallback(mqttCallback);

  while (!client.connected()) {
    // connect to Mqtt server and subcribe to order channel
    if (client.connect(MQTT_RFLINK_CLIENT_NAME)) {
      client.subscribe(MQTT_RFLINK_ORDER_CHANNEL);
      controlStatusLed(STATUS_LED_MQTT, HIGH);
      Serial.print(" OK");
    }
    else {
      controlStatusLed(STATUS_LED_MQTT, HIGH);
      delay(1000);
      controlStatusLed(STATUS_LED_MQTT, LOW);
      Serial.print(F("ERROR ("));
      Serial.print(client.state());
      Serial.println(F(")"));
      Serial.println(F("retry in 5 secs"));
      delay(4000);
    }
  }
  
  Serial.print(F(" ("));
  Serial.print(SERVER);
  Serial.println(F(")"));
  controlStatusLed(STATUS_LED_MQTT, HIGH);
  
  return client.connected();
}


/**
 * try to connect to Wifi
 */
void wifiConnect() {
  controlStatusLed(STATUS_LED_WIFI, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi..."));

  while(WiFi.status() != WL_CONNECTED){
    controlStatusLed(STATUS_LED_WIFI, HIGH);
    delay(200);
    controlStatusLed(STATUS_LED_WIFI, LOW);
  }
  Serial.print(F(" OK ("));
  Serial.print(WiFi.localIP());
  Serial.println(F(")"));
  controlStatusLed(STATUS_LED_WIFI, HIGH);
}


/**
 * Setup software serial to get RFlink output
 */
void initSoftwareSerial() {
  Serial.print(F("Init software serial..."));
  mySerial.begin(57600); // RF Link output at 57600
  Serial.println(F(" OK"));
}


void initStatusLeds() {
  pinMode(STATUS_LED_WIFI   , OUTPUT);
  pinMode(STATUS_LED_MQTT   , OUTPUT);
  pinMode(STATUS_LED_IN     , OUTPUT);
  pinMode(STATUS_LED_OUT    , OUTPUT);
  pinMode(STATUS_LED_OTA    , OUTPUT);
  pinMode(STATUS_LED_WEB    , OUTPUT);

  controlStatusLed(STATUS_LED_WIFI, LOW);
  controlStatusLed(STATUS_LED_MQTT, LOW);
  controlStatusLed(STATUS_LED_IN  , LOW);
  controlStatusLed(STATUS_LED_OUT , LOW);
  controlStatusLed(STATUS_LED_OTA , LOW);
  controlStatusLed(STATUS_LED_WEB , LOW);
}


void initOTA() {
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

  ArduinoOTA.onStart([]() { Serial.println(F("\n=== OTA Start ===")); controlStatusLed(STATUS_LED_OTA , HIGH); });
  ArduinoOTA.onEnd([]()   { Serial.println(F("\n=== OTA End ===")); controlStatusLed(STATUS_LED_OTA , LOW); });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    unsigned int percent = (progress / (total / 100));
    static int ledstate = LOW;
    
    Serial.printf("\tOTA Progress: %u%%\r", percent);
    if(percent%5 == 0) {
      ledstate == LOW ? ledstate=HIGH:ledstate=LOW;
      controlStatusLed(STATUS_LED_OTA, ledstate);
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });
  
  ArduinoOTA.begin();
}

void controlStatusLed(int pin, int state) {
  digitalWrite(pin, state);
}

/*********************************************************************************
 * Classic arduino bootstrap
/*********************************************************************************/
void setup() {

  initStatusLeds();
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
  }
  
  delay(1000);
  Serial.println(F("\n\n\n======= RFLINK TO JSON MQTT - ESP8266 ======="));
  
  initSoftwareSerial();
  wifiConnect();
  initOTA();
  mqttConnect();
  initWebServer();
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
    controlStatusLed(STATUS_LED_IN, HIGH);
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
      controlStatusLed(STATUS_LED_IN, LOW);
    }
  }
  
  client.loop();
  ArduinoOTA.handle();
  webSocket.cleanupClients();
  //webSocket.printfAll("web socket refresh \n");
  //delay(2000);
}
