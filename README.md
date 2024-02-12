# RflinkToJsonMqtt
This arduino sketch provides a gateway between RFlink (serial output) and a MQTT server (through WiFi). 

The serial message from Rflink is converted on the fly to JSON payload. This make Rflink output more standard and easier to parse.
The sketch also subscribes to a specific MQTT channel and listen to orders to be sent to the Rflink (no JSON, just plain Rflink text order).

The Original code was made for Arduino nano and W5100 ethernet. The main branch now runs only on ESP8266 and use wifi to connect to mqtt server. It provides OTA updates and HTTP server to see RF activcity on a browser. This code has been tested on "Wemos D1 lite" and "Node MCU". 

All parameters can be changed in file common.h
The sketch now uses the hardware serial to communicate with RFlink in order to avoid errors (software serial is not reliable at 57600 bauds on esp8266). 

You should use 4 leds to get a minimal user feeback for wifi connection, mqtt server connection, message in and message out.

Build example can be found on by blog, but the post has been made with the old wiring that used software serial to communicate with Rflink: https://www.bluemind.org/rflink-mqtt-v2-enhanced-minimized

**Main features:**
- Support all documented Rflink fields
- Convert HEX values to corresponding decimal int and float (eg: TEMP=00cb is converted to "TEMP":20.3)
- Replace "/" by "_" in field names to avoid unexpected mqtt channel path
- All non RF messages are ignored (boot message, ping, etc.)
- Web interface that reports activity : incomming RFlink message, Mqtt message and received Rflink order to transmit. All Log message are also printed on the browser's console

Some examples of Rflink message converted and sent to the mqtt server :
| Rflink message                                   | mqtt message                                        |
|--------------------------------------------------|-----------------------------------------------------|
| 20;97;OWL_CM119;ID=00EC;WATT=0456;KWATT=0079fa46;| rflink/OWL_CM119/00EC/{"WATT":1110,"KWATT":7993926} |
| 20;AE;OregonV1;ID=008B;TEMP=00cc;BAT=OK;         | rflink/OregonV1/008B/{"TEMP":20.4,"BAT":"OK"}       |
| 20;9C;NewKaku;ID=00000200;SWITCH=1;CMD=OFF;      | rflink/NewKaku/00000200/{"SWITCH":"1","CMD":"OFF"}  |

**Web interface screenshot:**
<img src="https://github.com/jit06/RflinkToJsonMqtt/blob/770418db8d2389958073e0d3b5f243b6ba7c8848/screenshot_web.jpg" width=920>

**Known limitations:**
- Hardware serial is swapped to D7 and D8, so they are dedicated to serial communication with RFLink
- Default USB Serial and TX/RX pins are mapped with software serial at 9600 bauds. So Serial debugging only works at that speed.

**Required libraries:**
- SoftwareSerial
- PubSubClient
- Wifi
- ESPAsyncWebServer 
- ESPAsyncTCP

**Default Wiring:**
- D0 (gpio16) => Wifi status led
- D2 (gpio4)  => Mqtt connection status led
- D5 (gpio14) => Rflink message status IN led
- D6 (gpio12) => Rflink message status OUT led
- D1 (gpio5)  => To arduino's RST pin via a 100 ohms resistor
- D7 (gpio13) => Rflink hardware Serial RX to Arduino Mega TX0
- D8 (gpio15) => Rflink hardware Serial TX to Arduino Mega RX0
