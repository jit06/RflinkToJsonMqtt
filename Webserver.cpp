#include <SoftwareSerial.h>
#include "Webserver.h"
#include "Common.h"
#include "StatusLeds.h"

// web server variables
AsyncWebServer webServer(HTTP_PORT);
AsyncWebSocket webSocket("/ws");

// from main sketch file
extern SoftwareSerial* Logger;

/*
 * send a standard HTTP not found response
 */
void httpNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


/*
 * send default HTTP page
 */
void httpRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", root_html);
}


/*
 * handle websocket events
 */
void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  Logger->println(F("== HTTP =="));

  switch(type) {
    case WS_EVT_CONNECT:
      Logger->print(F("web socket connected - ID = ")); Logger->println(client->id());
      client->printf("web socket connected - ID = %u\n\n", client->id());
      client->ping();
      break;

    case WS_EVT_DISCONNECT:
      Logger->print(F("web socket disconnected - ID = ")); Logger->println(client->id());
      break;

    case WS_EVT_ERROR:
      Logger->print(F("web socket error - ID = ")); Logger->println(client->id());
      Logger->print(F("ERROR = ")); Logger->println((char*)data);
      break;

    case WS_EVT_PONG:
      Logger->print(F("web socket pong - ID = ")); Logger->println(client->id());
      break;

    case WS_EVT_DATA :
      Logger->print(F("web socket data (ignored) - ID = ")); Logger->println(client->id());
      break;  
  }
}


/*
 * Setup HTTP serveur and socket
 */
void initWebServer() {
   Logger->print(F("starting web server..."));
   webServer.on("/"  ,HTTP_GET,httpRoot);
   webServer.onNotFound(httpNotFound);
   webSocket.onEvent(onWebSocketEvent);
   webServer.addHandler(&webSocket);
   webServer.begin();
   Logger->println(F(" OK"));
}
