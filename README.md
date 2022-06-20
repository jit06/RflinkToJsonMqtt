# RflinkToJsonMqtt
This arduino sketch provides a gateway between RFlink (serial output) and a MQTT server (through ethernet). 

The serial message from Rflink is converted on the fly to json payload. This make Rflink output more standard and easier to parse.
The sketch also subscribes to a specific MQTT channel and listDen to orders to be sent to the Rflink (no json, just plain Rflink text order).

The Original code was made for Arduino nano and W5100 ethernet. The main branch now runs only on ESP8266 and use wifi to connect to mqtt server. It provides OTA updates and HTTP server to see RF activcity on a browser. 

All parameters can be changed in file common.h

Build example can be found on by blog : https://www.bluemind.org/rflink-mqtt-v2-enhanced-minimized

Known limitations :
- setting Rlink to debug mode crash this sketch (debug messages are too long for buffer size)
- not all possible fields / values has been tested, you may want to customize Rflink.h (feel free to make pull request)

Required libraries :
- SoftwareSerial
- PubSubClient
- Wifi
- ESPAsyncWebServer 
- ESPAsyncTCP


Exemple of wiring:
D1 (gpio5) => software serial TX
D2 (gpio4) => software serial RX

D5 (gpio14) => oled D0 (pin3)
D6 (gpio12) => oled DC (MISO, pin6)
D7 (gpio13) => oled D1 (MOSI, pin4)
D8 (gpio15) => oled CS (chip select, pin7)
