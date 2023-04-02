# WebSocketSerialMonitor
ESP8266 based serial-to-websockets proxy for OTA debugging.

Forked from https://github.com/tzapu/WebSocketSerialMonitor.

Using this sketch and an extra ESP8266 module you can monitor the serial output of your ESP8266, Arduino or any other serial outputing device Over-the-Air, in a browser.

### Getting started
- upload this sketch to your proxy ESP8266, the device you will attach to the serial terminals of the device to be monitored. make sure to set serial baud rate to be the same as your monitored device. Any 3rd party libs used are mentioned in the file.
- connect the proxy module to your WiFi. It uses WiFiManager so if it can't connect it will start and access point for you to configure it.
- connect the proxy ground to the monitored device ground and the proxy RX pin to the monitored device TX pin

### Install web page into LittleFS
- follow installation steps from https://github.com/earlephilhower/arduino-esp8266littlefs-plugin
- ensure HTM file is present inside data folder
- after uploading the sketch, use **Tools > ESP8266 LittleFS Data Upload**

### Open Serial Monitoring page
- web page can be accessed from http://ip_address_of_esp/
  - **WebSocket Host** should automatically populate with ws://ip_address_of_esp:81/ws
- alternatively, open or download (from gh-pages branch) and open http://tzapu.github.io/WebSocketSerialMonitor/ and connect to your proxy module's ip on port 81

That's it, you should see live serial data streaming from your devices.

### Future plans
(if this proves useful to people)
- add control of gpio pins so you can reset monitored module for instance
- enable switching of baud rates on the fly
- make the web page nicer and more app like
- add MDNS for local name resolution
- compress and concatenate files to be served straight from spiffs
- add ability to download console page straight to spiffs on first connect, and update later on
