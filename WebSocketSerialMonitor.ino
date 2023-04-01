/*
   WebSocketServer.ino
   Modified from https://github.com/tzapu/WebSocketSerialMonitor
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>   //https://github.com/Links2004/arduinoWebSockets/
#include <Hash.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager

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


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset settings - for testing
  //wifiManager.resetSettings();

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("START MIRRORING SERIAL");
  Serial.println(WiFi.localIP());

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
       // clear the string:
    inputString = "";
    stringComplete = false;

    //line += '\n';
    webSocket.broadcastTXT(line);
    Serial.println(line);
  }
  webSocket.loop();
  
}
