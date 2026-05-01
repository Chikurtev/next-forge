// ===== TASK BOARD MEASUREMENT SYSTEM =====
// Smart Electronic Task Board Controller for M5 Stick S3
// Measures manipulation performance with sensor data and MQTT telemetry
// First written by Peter So, December 2020.
// Last updated July 2024, ported to M5Stick S3 May 2026
//
// REQUIRES: PbHub unit connection via I2C
// DEFAULT: Attempts WiFi network connection on power up
// OFFLINE: Hold M5 button during power up to use without WiFi

// ===== INCLUDE LIBRARIES =====
#include <M5Unified.h>
#include <Preferences.h>
#include <Wire.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

// ===== INCLUDE CUSTOM MODULES =====
#include "config.h"
#include "porthub.h"
#include "display.h"
#include "task_checks.h"
#include "trial_logic.h"
#include "hardware.h"
#include "telemetry.h"
#include "input_handler.h"
#include "persistence.h"


// ===== GLOBAL OBJECTS =====
Preferences myPrefs;
PortHub porthub;
WiFiClient espClient;
PubSubClient client(espClient);

// ===== HUB ADDRESSING =====
uint8_t HUB_ADDR[6] = { HUB1_ADDR, HUB2_ADDR, HUB3_ADDR, HUB4_ADDR, HUB5_ADDR, HUB6_ADDR };


// ===== TIMING VARIABLES =====
unsigned long now = 0;
unsigned long last = 0;
unsigned long scan_time = 0;
unsigned long startTime = 0;
volatile unsigned long usecCount = 0;

// ===== DISPLAY & CONTROL VARIABLES =====
int screenSelector = 0;

// ===== TRIAL STATE VARIABLES =====
int forceStop = 0;
int trialCounter = 0;
int humanAttempts = 0;
unsigned long trialTime = 0;
int wifiEnabled = 0;
int countStart = 0;
int trialRunning = 0, timeLeft = 0, ptsCollected = 0;
int verbose = VERBOSE;

// ===== TASK LATCH VARIABLES =====
int buttonPushLatch = 0, faderLatch = 0, angleLatch = 0, angleDoorLatch = 0;
int cableWrapProbeStowLatch = 0, cableWrapLatch = 0, probeGoalLatch = 0;
int OP180_1_Latch = 0, OP180_2_Latch = 0, trialCompletedLatch = 0;
int faderLatch2 = 0;
int faderGoal2 = 0;
int humanStart = 0;

// ===== BUTTON STATE VARIABLES =====
int startBtnState = -1, stopBtnState = -1, resetBtnState = -1;
int buttonPushState = -1, faderValue = -1;
int angleValue = -1;
int probeStartState = -1, probeGoalState = -1;
int OP180_1_State = -1, OP180_2_State = -1;

// ===== BUTTON STATE OLD VALUES =====
int buttonPushState_old = -1, stopBtnState_old = -1, faderValue_old = -1;
int angleValue_old = -1, probeStartState_old = -1, probeGoalState_old = -1;
int OP180_1_State_old = -1, OP180_2_State_old = -1;

// ===== TIMESTAMP VARIABLES =====
int TS_button = 0, TS_fader_mid = 0, TS_fader = 0, TS_angle = 0, TS_angle_door = 0;
int TS_cableWrap = 0, TS_cableWrapProbeStow = 0, TS_probeGoal = 0;
int TS_OP180_1 = 0, TS_OP180_2 = 0, TS_finalTrialTime = 0;

// ===== SENSOR DATA VARIABLES =====
float force = 0.0F, cumForce = 0.0F;
float startaccX = 0.0F, startaccY = 0.0F, startaccZ = 0.0F;
float accX = 0.0F, accY = 0.0F, accZ = 0.0F;
float gyroX = 0.0F, gyroY = 0.0F, gyroZ = 0.0F;

// ===== DISPLAY VARIABLES =====
int display[4] = { 0 };  // min, sec, msec, usec

// ===== WIFI & APP VARIABLES =====
String task_board_ssid, unique_ssid;

// ===== SETUP FUNCTION =====
void setup() {
  // initialize the M5Stack object
  M5.begin();
  porthub.begin();
  Serial.begin(115200);
  delay(250);  //give serial some time to startup

  // // GPIO setting
  pinMode(LED_PIN, OUTPUT);  //GPIO19 for M5Stick S3 builtin LED

  // Read values from non-volatile memory
  myPrefs.begin("task-board", true);  // defines the namespace used to enable persistent memory reads
  Serial.printf("reading values from NVM into run-time memory...");
  trialCounter = myPrefs.getUInt("trialCounter", 0);    //assign run-time var from NVM
  humanAttempts = myPrefs.getUInt("humanAttempts", 0);  //assign run-time var from NVM
  Serial.printf("trialCounter:%d, humanTrialsCount:%d\n", trialCounter, humanAttempts);
  myPrefs.end();  //close namespace
  Serial.printf("done\n");
  if (M5.BtnB.isPressed()) {  //press and hold BtnB during power on to reset the trial counters in persistent memory
    Serial.printf("clearing NVM key-pair values.\n");
    myPrefs.begin("task-board", false);
    myPrefs.clear();
    myPrefs.end();
    trialCounter = 0;
    humanAttempts = 0;
  }

  // // Print out the device's unique MAC address
  Serial.print("ESP Board MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Build string for SSID
  task_board_ssid = String("AutoConnect_");
  unique_ssid = String();
  unique_ssid = task_board_ssid + TOKEN.c_str();

  // Configure Lcd display setup
  M5.Display.setRotation(3);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(0, 5);

  // // Setup WiFi connection or boot in LOCAL MODE
  if (M5.BtnA.isPressed() != 1)  // this should be != 1 so that the default behavior is to connect to WiFi
  {
    M5.Display.print(" Smart Task Board ");
    M5.Display.printf("v%s\n ", FW_VERSION);
    M5.Display.printf("TOKEN:%s\n\n", TOKEN.c_str());
    M5.Display.print(" Set default WiFi by connecting to board\n");
    M5.Display.print(" SSID with a PC then browse to \n");
    M5.Display.print(" \"192.168.4.1\" and select preferred\n");
    M5.Display.print(" WiFi network and enter password.\n");
    M5.Display.print(" Board will then attempt to autoconnect.\n\n");
    M5.Display.printf(" Task Board SSID: \n %s\n", unique_ssid.c_str());
    M5.Display.printf(" Password: %s\n\n", TASK_BOARD_PW);
    M5.Display.print(" Connecting to last saved WiFi SSID...\n");
    M5.Display.print(" Use w/o WiFi:Power while holding M5 btn");
    wifiEnabled = 1;  //replace this with res variable

    //Wifi Manager Config START
    WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP

    Serial.printf("Starting wifimanager...\n");
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    update_inputs();
    //reset stored wifi settings - wipe credentials for testing
    if (porthub.hub_d_read_value_A(HUB_ADDR[0]) == 0 && porthub.hub_d_read_value_B(HUB_ADDR[0]) == 0 ){ // plug in device while holding down both the blue and red buttons to reset the stored wifi settings
      Serial.printf("Resetting wifi manager stored settings...");
      wm.resetSettings(); //leave commented
      M5.Display.fillScreen(BLACK);
      M5.Display.setCursor(5,5);
      M5.Display.printf("Resetting wifi manager stored settings\n Power cycle device!");
      turn_LED_on();
      Serial.printf("reset done!\n");
    }
    

    bool res;                                                  // connection status of the wifimanager
    res = wm.autoConnect(unique_ssid.c_str(), TASK_BOARD_PW);  // password protected ap

    if (!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
    } else {
      //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");
    }
    //Wifi Manager Config END
    Serial.printf("End of wifimanager code block.\n");

    // Setup wireless connection
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setBufferSize(8192);  // Increase limit, this line fixed problem for my device

    //Check for new firmware
    turn_LED_on();
    client.setCallback(callback);
    initServerConnection();
    delay(1000);
    reportCurrentFirmwareVersion();
    requestNewFirmware();
    turn_LED_off();  //turn off LED

  } else {  // Press and hold M5 Button during power up to enter LOCAL Mode
    M5.Display.setCursor(5, 5);
    M5.Display.setTextSize(2);
    M5.Display.print("Booting Local Mode, no WiFi!");
    Serial.printf("Booting Local Mode, no WiFi!\n");
    turn_LED_off();  //turn off LED
    wifiEnabled = 0;
    delay(1000);
  }

  M5.Display.fillScreen(BLACK);  // clear screen
}

void loop() {
  // put your main code here, to run repeatedly:
  last = now;
  now = millis();  //maybe this could replace the interrupt timer?
  scan_time = now - last;
  if (verbose == 1) {
    Serial.printf("scan timer: %dms\n", scan_time);
  }

  // main function refactor
  update_inputs();  //required
  if (trialRunning) {
    // update trialTime variable in telemetry while trial is running
    trialTime = now - startTime;  // trial time in milliseconds
    usecCount = trialTime * 1000UL;

    //time calculation
    display[1] = (int)((trialTime / 1000) % 60);     // seconds
    display[0] = (int)((trialTime / 1000 / 60) % 3600);  // minutes
    display[2] = (int)(trialTime % 1000);               // milliseconds
  } else {
    display[2] = 0;
  }
  check_blue_button();  //minimal timing impact
  check_slider();       //minimal timing impact
  check_probe_plug();   //minimal timing impact
  check_door();         //minimal timing impact
  // check_TB_circuit(); //empty and causing crashes when uncommented
  check_cable_posts();          //minimal timing impact
  check_probe_tip_holder();     //minimal timing impact
  check_trialStartStopLogic();  //there is something in here that blows up the scan time <==
  update_screen();              //+20ms when isolated
  send_telemetry();             //+5ms when isolated

  //print out seconds to the serial monitor
  if (verbose == 1) {
    Serial.printf("DeviceToken:%s, State:Btn:%d,Fader:%d,Angle:%d,ProbeInserted:%d,CircuitProbed:%d,Post1:%d,Post2:%d, Protocol:%s, Batt:%d, TrialRunning:%d, TimeLeft_sec:%d, TrialPts:%d, TotalTrialForce:%0.2f, Time_us:%d\n", TOKEN.c_str(), buttonPushState, faderValue, angleValue, probeStartState, probeGoalState, OP180_1_State, OP180_2_State, PROTOCOL_ID, M5.Power.getBatteryVoltage(), trialRunning, timeLeft, ptsCollected, cumForce, usecCount);
  }
  //print the scan time to the m5 screen
  M5.Display.setCursor(175, 5);
  M5.Display.setTextSize(2);
  M5.Display.printf("%d", scan_time);
}
