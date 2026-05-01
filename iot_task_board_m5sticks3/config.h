#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

// ===== APPLICATION SETTINGS =====
#define TASK_BOARD_PW "robothon"
#define PROTOCOL_ID "ROBOTHON_2023"

// ===== TASK BOARD SPECIFIC SETTINGS =====
const String TOKEN = "task_board_dev";
const String APP_VERSION = "c1v9jqmgul2l1s47m6bg-v0";
const String FW_VERSION = "1.1.1";

// ===== TIMING SETTINGS =====
#define TIMELIMIT 600           // Trial Time Limit in seconds (600 is 10min)
#define FADERSP 2000            // Fader setpoint
#define FADERSP2 1500           // Fader setpoint 2
#define FADERTOLERANCE 60       // Fader tolerance
#define ANGLESP 2400            // Angle setpoint - Make sure this works for all task boards

// ===== POINTS SETTINGS =====
#define PTS_BUTTON 1
#define PTS_FADER 1
#define PTS_CIRCUIT_PROBE 1
#define PTS_CABLEWRAP 1
#define PTS_PROBEINSERT 1

// ===== BUTTON & LED STATES =====
#define BUTTON_ON 0
#define BUTTON_OFF 1
#define LED_PIN 21  // GPIO21 for M5Stick S3 builtin LED

// ===== DISPLAY SETTINGS =====
// Note: Display size is 240x135 pixels for M5Stick S3

// ===== MQTT SETTINGS =====
#define MQTT_SERVER "mqtt.cloud.kaaiot.com"
#define MQTT_PORT 1883

// ===== TELEMETRY RATE SETTINGS =====
#define TRIAL_PUBLISH_RATE 1000UL      // 1000ms
#define FIVE_SECONDS 5000UL             // 5000ms

// ===== DEBUG SETTINGS =====
#define VERBOSE 0  // Set to 1 to enable serial output

#endif
