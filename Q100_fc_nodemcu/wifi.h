#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "html.h";


const char* ssid     = "JOJO";
const char* password = "12345678";
String message = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String processor(const String& var){
   // Serial.println(var);
    if(var == "STATE"){
      return "JOJO";
    }
    return String();
}

void notifyClients() {
  ws.textAll("Roll: "+String(gyro_roll_input)+" | Pitch: "+String(gyro_pitch_input)+" | Yaw: "+String(gyro_yaw_input));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    if (message.indexOf("remote:") >= 0) {
       // Serial.println(message);
        notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        //Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
   }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
  
void initRadio(){
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
//    Serial.print("AP IP address: ");
//    Serial.println(IP);
//    Serial.println(WiFi.localIP());
    initWebSocket();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html, processor);
    });
    server.begin();
}
