#ifndef H_WEBSERVER
#define H_WEBSERVER

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>


void httpNotFound(AsyncWebServerRequest *request);
void httpRoot(AsyncWebServerRequest *request);
void webSocketEventHandler(void *arg, uint8_t *data, size_t len);
void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void initWebServer();

const char root_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html style="height:100%">
<head>
  <title>Rflink to Json MQTT</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    #dateTime, #rawMessage, #mqttMessage {
      height:100%;
      padding:5px;
      display:inline-block;
    }

    div {
      overflow:scroll;
      width:100%;
      padding:0px;
      margin:0px;
    }
  </style>
</head>
<body style="height:100%;overflow:hidden">
  <h1>Rflink to Json MQTT</h1><pre id="httpMessage" style="position:absolute;top:20px;left:350px"></pre>
  <div style="height:70%;background-color:#DDD;">
    <pre id="dateTime"    style="width:175px;" ></pre>
    <pre id="rawMessage"  style="width:40%;" ></pre>
    <pre id="mqttMessage" style="width:40%;"></pre>
  </div>
  <div style="height:20%;background-color:#EEE;">
    <pre id="cmdMessage" style="width:auto;height:100%;padding:5px"></pre>
  </div>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    var buffer="";
  
    window.addEventListener('load', initWebSocket);
    
    function initWebSocket() {
      console.log('Open WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = function() { console.log('Connection opened');};
      websocket.onclose   = function() { console.log('Connection closed'); setTimeout(initWebSocket,2000) };
      websocket.onmessage = onMessage;
    }
    
    function onMessage(event) {
      buffer+=event.data
      now = new Date();
      
      if(event.data.lastIndexOf("\n") >= 0) {
        console.log(buffer);
      
        if(buffer.indexOf("raw << ")>=0) {
          node = document.getElementById('rawMessage');
          buffer=buffer.substring(7);

          document.getElementById('dateTime').innerText += now.toLocaleString() +"\n";
        }
        else if(buffer.indexOf("mqtt >> ")>=0) {
          buffer=buffer.replace("== RFLINK ==","");
          node = document.getElementById('mqttMessage');
          buffer=buffer.substring(8);
        }
        else if(buffer.indexOf("web socket")>=0) {
          node = document.getElementById('httpMessage');
        }
        else if(buffer.indexOf("msg << ")>=0) {
          node = document.getElementById('cmdMessage');
          buffer=buffer.substring(7);
        }
        else {
          buffer="";
          return;
        }
        
        node.innerText += buffer;
        buffer = "";
      }
      
      len = node.innerText.length;
      if(len >= 5000) {
        if(node.id == "cmdMessage") {
            node.innerText = node.innerText.substring(node.innerText.indexOf('\n')+1);
        }
        else {
          for(const i of ["dateTime", "rawMessage", "mqttMessage" ] ) {
            el = document.getElementById(i);
            el.innerText = el.innerText.substring(el.innerText.indexOf('\n')+1);
          }
        }
      }
    }
  </script>
</body></html>
)rawliteral";

#endif
