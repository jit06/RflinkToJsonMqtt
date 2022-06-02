# RflinkToJsonMqtt
This arduino sketch allow to build a gateway RFlink serial output and a MQTT server. 

The serial message from Rflink is converted on the fly to json payload. This make Rflink output more standard and easier to parse.
The sketch also subscribes to a specific MQTT channel and listDen to orders to be sent to the Rflink (no json, just plain Rflink text order).

This branch is dedicated to ESP8266 version and use wifi network to connect to mqtt server
All parameters can be changed in file common.h
Through, in order to avoid out of memory, it is not advised to modify BUFFER_SIZE, MAX_DATA_LEN nor MAX_CHANNEL_LEN

Known limitations :
- setting Rlink to debug mode crash this sketch (debug messages are too long for buffer size)
- not all possible fields / values has been tested, you may want to customize Rflink.h (feel free to make pull request)

Required libraries :
- SoftwareSerial
- PubSubClient

Exemple of wiring:
D1 (gpio5) => software serial TX
D2 (gpio4) => software serial RX

D5 (gpio14) => oled D0 (pin3)
D6 (gpio12) => oled DC (MISO, pin6)
D7 (gpio13) => oled D1 (MOSI, pin4)
D8 (gpio15) => oled CS (chip select, pin7)