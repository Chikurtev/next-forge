#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi and MQTT functions
String setup_wifi();
void connectWiFi();
void initServerConnection();
void reconnect();
void subscribeToCommand();
void callback(char* topic, byte* payload, unsigned int length);

// OTA update functions
void reportCurrentFirmwareVersion();
void requestNewFirmware();
void handleOtaUpdate(char* topic, byte* payload, unsigned int length);

// Telemetry functions
void publish_telemetry();
void send_telemetry();

#endif
