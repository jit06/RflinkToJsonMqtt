# RflinkToJsonMqtt
PLEASE NOTE THAT THIS BRANCH IS NOT UPDATED ANYMORE AS THE NEW VERSION HAS BEEN REWORKED FOR ESP8266
H0WEVER, THE CODE STILL COMPILES AND RUNS AND ARDUINO NANO AND WIZNET W5100

This arduino sketch allow to build a gateway RFlink serial output and a MQTT server. 

The serial message from Rflink is converted on the fly to json payload. This make Rflink output more standard and easier to parse.
The sketch also subscribes to a specific MQTT channel and listen to orders to be sent to the Rflink (no json, just plain Rflink text order).

This sketch has been tested during more than 10 days with around 10 messages per minutes on an arduino nano and a mini Wiznet (W5100) ethernet shield. It may support more messages.

Rflink serial output must be connected to pin 8 (rx) and 9 (tx).
All other parameters can be changed in file common.h
Through, in order to avoid out of memory, it is not advised to modify BUFFER_SIZE, MAX_DATA_LEN nor MAX_CHANNEL_LEN

Known limitations :
- setting Rlink to debug mode crash this sketch (debug messages are too long for buffer size)
- not all possible fields / values has been tested, you may want to customize Rflink.h (feel free to make pull request)

Required libraries :
- SoftwareSerial
- PubSubClient
- Ethernet

More detail on how to build the complete shield on my blog : http://www.bluemind.org/custom-arduino-shield-mqtt-rflink
