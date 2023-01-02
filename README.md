# RflinkToJsonMqtt
This arduino sketch provides a gateway between RFlink (serial output) and a MQTT server (through WiFi). 

The serial message from Rflink is converted on the fly to JSON payload. This make Rflink output more standard and easier to parse.
The sketch also subscribes to a specific MQTT channel and listen to orders to be sent to the Rflink (no JSON, just plain Rflink text order).

The Original code was made for Arduino nano and W5100 ethernet. The main branch now runs only on ESP8266 and use wifi to connect to mqtt server. It provides OTA updates and HTTP server to see RF activcity on a browser. 

All parameters can be changed in file common.h
The sketch now use the hardware serial to communicate with RFlink in order to avoid errors (software serial is not reliable at 57600 on esp8266)

You should use 5 leds to get a minimal user feeback for wifi connection, mqtt server connection, message in, message out and websocket / OTA.

Build example can be found on by blog, but the post has been made with the old wiring that used software serial to communicate with Rflink: https://www.bluemind.org/rflink-mqtt-v2-enhanced-minimized

**Known limitations :**
- not all possible fields / values has been tested, you may want to customize Rflink.h (feel free to make pull request)
- Hardware serial is swapped to D7 and D8, so they are dedicated to serial communication with RFLink
- Default USB Serial and TX/RX pins are mapped with software serial at 9600 bauds. So Serial debugging only works at that speed.

**Required libraries :**
- SoftwareSerial
- PubSubClient
- Wifi
- ESPAsyncWebServer 
- ESPAsyncTCP


**Default Wiring : **
D0 (gpio16) => Wifi status led
D1 (gpio5)  => Mqtt connection status led
D2 (gpio4)  => Rflink message status IN led
D5 (gpio14) => Rflink message status OUT led
D6 (gpio12) => Websocket connection status and OTA connection led

D7 (gpio13) => Rflink hardware Serial RX to Arduino Mega TX0
D8 (gpio15) => Rflink hardware Serial TX to Arduino Mega RX0
