/*
   WebSocketServer.ino
   Modified from https://github.com/tzapu/WebSocketSerialMonitor
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>   //https://github.com/Links2004/arduinoWebSockets/
#include <Hash.h>

#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include <ESP8266WebServer.h>
//#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager

#include "LittleFS.h"

// Replace with your network credentials
#include "passwords.h"
// const char* ssid = "ssid";
// const char* password = "password";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create WebSocketsServer object on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %s url: %s\n", num, ip.toString().c_str(), payload);

        // send message to client
        String payload = "Connected to Serial on " + WiFi.localIP().toString() + "\n";
        webSocket.sendTXT(num, payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
      hexdump(payload, lenght);

      // send message to client
      // webSocket.sendBIN(num, payload, lenght);
      break;
  }

}



// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  Serial.println(WiFi.localIP());
}



String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();
  
  initWiFi();
  initFS();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.htm", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");
  
  // Start server
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("START MIRRORING SERIAL");

  inputString.reserve(256);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      return;
    } else {
      inputString += inChar;
    }
  }
}


void loop() {
  serialEvent();
  if (stringComplete) {
    
    String line = inputString;
    // Clear the string:
    inputString = "";
    stringComplete = false;

    //line += '\n';
    webSocket.broadcastTXT(line);
    Serial.println(line);
  }
  webSocket.loop();
  
}
