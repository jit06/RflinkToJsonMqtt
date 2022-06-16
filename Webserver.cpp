#include "Webserver.h"
#include "Common.h"

// web server
AsyncWebServer webServer(HTTP_PORT);
AsyncWebSocket webSocket("/ws");

void httpNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void httpRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", root_html);
}

void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  Serial.println(F("\n=== HTTP ==="));

  switch(type) {
    case WS_EVT_CONNECT:
      Serial.print(F("web socket connected - ID = ")); Serial.println(client->id());
      client->printf("web socket connected - ID = %u\n\n", client->id());
      client->ping();
      break;
    
    case WS_EVT_DISCONNECT:
      Serial.print(F("web socket disconnected - ID = ")); Serial.println(client->id());
      break;
  
    case WS_EVT_ERROR:
      Serial.print(F("web socket error - ID = ")); Serial.print(client->id());
      Serial.print(F("ERROR = ")); Serial.println((char*)data);
      break;

    case WS_EVT_PONG:
      Serial.print(F("web socket pong - ID = ")); Serial.println(client->id());
      break;
    
    case WS_EVT_DATA :
      Serial.print(F("web socket data (ignored) - ID = ")); Serial.println(client->id());
      break;  
  }
}

void initWebServer() {    
   Serial.print(F("starting web server..."));
   webServer.on("/"  ,HTTP_GET,httpRoot);
   webServer.onNotFound(httpNotFound);
   webSocket.onEvent(onWebSocketEvent);
   webServer.addHandler(&webSocket);
   webServer.begin();
   Serial.println(F(" OK"));
}
