# RflinkToJsonMqtt
This arduino sketch allow to build a gateway RFlink serial output and a MQTT server. 

The serial message from Rflink is converted on the fly to json payload. This make Rflink output more standard and easier to parse.
The sketch also subscribes to a specific MQTT channel and listen to orders to be sent to the Rflink (no json, just plain Rflink text order).

This branch is dedicated to ESP8266 version and use wifi network to connect to mqtt server
All parameters can be changed in file common.h
Through, in order to avoid out of memory, it is not advised to modify BUFFER_SIZE, MAX_DATA_LEN nor MAX_CHANNEL_LEN

Known limitations :
- setting Rlink to debug mode crash this sketch (debug messages are too long for buffer size)
- not all possible fields / values has been tested, you may want to customize Rflink.h (feel free to make pull request)

Required libraries :
- SoftwareSerial
- PubSubClient