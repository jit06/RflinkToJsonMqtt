#ifndef H_WEBSERVER
#define H_WEBSERVER

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebSocket webSocket;
extern AsyncWebServer webServer;

void httpNotFound(AsyncWebServerRequest *request);
void httpRoot(AsyncWebServerRequest *request);
void webSocketEventHandler(void *arg, uint8_t *data, size_t len);
void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void initWebServer();

const char root_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Rflink to Json MQTT</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h1>Rflink to Json MQTT</h1>
  <pre id="message"></pre>

  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
  
    window.addEventListener('load', initWebSocket);
    
    function initWebSocket() {
      console.log('Open WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = function() { console.log('Connection opened');};
      websocket.onclose   = function() { console.log('Connection closed'); setTimeout(initWebSocket,2000) };
      websocket.onmessage = onMessage;
    }
    
    function onMessage(event) {
      document.getElementById('message').innerText += event.data;
    }
  </script>
</body></html>
)rawliteral";

#endif
