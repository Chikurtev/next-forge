#include "telemetry.h"
#include "config.h"
#include <WiFiClient.h>
#include <HTTPUpdate.h>
#include <M5StickCPlus2.h>

// External WiFi client
extern WiFiClient espClient;
extern PubSubClient client;

// External variables for telemetry
extern int accX, accY, accZ, gyroX, gyroY, gyroZ;
extern int trialCounter, humanAttempts, faderValue, angleValue, startBtnState, resetBtnState;
extern int buttonPushState, stopBtnState, probeStartState, probeGoalState, OP180_1_State, OP180_2_State;
extern int trialRunning, trialTime, TS_button, TS_fader_mid, TS_fader, TS_angle, TS_angle_door;
extern int TS_cableWrap, TS_cableWrapProbeStow, TS_probeGoal, TS_finalTrialTime;
extern float cumForce;
extern int ptsCollected;
extern unsigned long scan_time;
extern int humanStart;
extern int wifiEnabled;
extern int verbose;
extern String unique_ssid;

unsigned long lastPublish = 0;

String setup_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println();
    Serial.printf("Connecting to [%s]", unique_ssid.c_str());
    WiFi.begin(unique_ssid.c_str(), TASK_BOARD_PW);
    connectWiFi();
  }
  return "WiFi setup complete";
}

void connectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initServerConnection() {
  setup_wifi();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    char* client_id = "client-id-123ab";
    if (client.connect(client_id)) {
      Serial.println("Connected to WiFi");
      subscribeToCommand();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void subscribeToCommand() {
  String topic = "kp1/" + APP_VERSION + "/cex/" + TOKEN + "/command/SWITCH/status";
  client.subscribe(topic.c_str());
  Serial.println("Subscribed on topic: " + topic);

  String serverPushOnConnect = "kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN + "/config/json/#";
  client.subscribe(serverPushOnConnect.c_str());
  Serial.println("Subscribed to server firmware push on topic: " + serverPushOnConnect);

  String serverFirmwareResponse = "kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN + "/config/json/status/#";
  client.subscribe(serverFirmwareResponse.c_str());
  Serial.println("Subscribed to server firmware response on topic: " + serverFirmwareResponse);

  String serverFirmwareErrorResponse = "kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN + "/config/json/status/error";
  client.subscribe(serverFirmwareErrorResponse.c_str());
  Serial.println("Subscribed to server firmware response on topic: " + serverFirmwareErrorResponse);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("\nHandling command message on topic: %s\n", topic);

  if (String(topic).startsWith("kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN)) {
    handleOtaUpdate(topic, payload, length);
    return;
  }

  if (!String(topic).startsWith("kp1/" + APP_VERSION + "/cex/" + TOKEN)) {
    return;
  }

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, payload, length);
  JsonVariant json_var = doc.as<JsonVariant>();

  DynamicJsonDocument commandResponse(2048);
  for (int i = 0; i < json_var.size(); i++) {
    unsigned int command_id = json_var[i]["id"].as<unsigned int>();
    commandResponse.createNestedObject();
    commandResponse[i]["id"] = command_id;
    commandResponse[i]["statusCode"] = 200;
    commandResponse[i]["payload"] = "done";
  }

  String responseTopic = "kp1/" + APP_VERSION + "/cex/" + TOKEN + "/result/SWITCH";
  client.publish(responseTopic.c_str(), commandResponse.as<String>().c_str());
  Serial.println("Published response to SWITCH command on topic: " + responseTopic);
}

void reportCurrentFirmwareVersion() {
  String reportTopic = "kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN + "/applied/json";
  String reportPayload = "{\"configId\":\"" + FW_VERSION + "\"}";
  Serial.println("Reporting current firmware version on topic: " + reportTopic + " and payload: " + reportPayload);
  client.publish(reportTopic.c_str(), reportPayload.c_str());
}

void requestNewFirmware() {
  int requestID = random(0, 99);
  String firmwareRequestTopic = "kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN + "/config/json/" + requestID;
  Serial.println("Requesting firmware using topic: " + firmwareRequestTopic);
  client.publish(firmwareRequestTopic.c_str(), "{\"observe\":true}");
}

void handleOtaUpdate(char* topic, byte* payload, unsigned int length) {
  Serial.printf("\nHandling firmware update message on topic: %s and payload: ", topic);

  if (!String(topic).startsWith("kp1/" + APP_VERSION + "/cmx_ota/" + TOKEN)) {
    return;
  }

  DynamicJsonDocument doc(1023);
  deserializeJson(doc, payload, length);
  JsonVariant json_var = doc.as<JsonVariant>();
  Serial.println(json_var.as<String>());
  
  if (json_var.isNull()) {
    Serial.println("No new firmware version is available");
    return;
  }

  unsigned int statusCode = json_var["statusCode"].as<unsigned int>();
  if (statusCode != 200) {
    Serial.printf("Firmware message's status code is not 200, but: %d\n", statusCode);
    return;
  }

  String firmwareLink = json_var["config"]["link"].as<String>();
  t_httpUpdate_return ret = httpUpdate.update(espClient, firmwareLink.c_str());

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void publish_telemetry() {
  DynamicJsonDocument telemetry(8192);
  JsonObject root = telemetry.to<JsonObject>();

  root["accX"] = accX * 1000;
  root["accY"] = accY * 1000;
  root["accZ"] = accZ * 1000;
  root["gyroX"] = gyroX;
  root["gyroY"] = gyroY;
  root["gyroZ"] = gyroZ;
  root["trialCounter"] = trialCounter;
  root["HumanAttempts"] = humanAttempts;
  root["faderValue"] = faderValue;
  root["angleValue"] = angleValue;
  root["startButtonState"] = startBtnState;
  root["resetButtonState"] = resetBtnState;
  root["pushButtonState"] = buttonPushState;
  root["stopButtonState"] = stopBtnState;
  root["probeStartState"] = probeStartState;
  root["probeGoalState"] = probeGoalState;
  root["WrapPostLeftState"] = OP180_1_State;
  root["WrapPostRightState"] = OP180_2_State;
  root["trialStarted"] = trialRunning;
  root["trialTime"] = trialTime;
  root["Time_ButtonPressed"] = TS_button;
  root["Time_Button"] = TS_button;
  root["Time_Fader"] = TS_fader;
  root["Time_FaderSP1Achieved"] = TS_fader_mid;
  root["Time_FaderSP2Achieved"] = TS_fader;
  root["Time_TerminalBlockProbed"] = TS_angle;
  root["Time_Angle"] = TS_angle;
  root["Time_Door_Opened"] = TS_angle_door;
  root["Time_CableWrap"] = TS_cableWrap;
  root["Time_CableWrapped"] = TS_cableWrap;
  root["Time_CableWrappedProbeTipReplaced"] = TS_cableWrapProbeStow;
  root["Time_ProbePlugInserted"] = TS_probeGoal;
  root["Time_ProbeGoal"] = TS_probeGoal;
  root["Time_FinalTrialTime"] = TS_finalTrialTime;
  root["cumForce"] = cumForce;
  root["trialPoints"] = ptsCollected;
  root["battVoltage(V)"] = StickCP2.Power.getBatteryVoltage() / 1000;
  root["battCurrent(mA)"] = StickCP2.Power.getBatteryCurrent();
  root["M5BattLevel"] = StickCP2.Power.getBatteryLevel();
  root["M5IsCharging"] = StickCP2.Power.isCharging();
  root["HumanStartFlag"] = humanStart;
  root["M5ScanTime"] = scan_time;

  String topic = "kp1/" + APP_VERSION + "/dcx/" + TOKEN + "/json";
  client.publish(topic.c_str(), telemetry.as<String>().c_str());
  Serial.println("Published on topic: " + topic);
}

void send_telemetry() {
  if (wifiEnabled == 1) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    if (trialRunning == 1 && millis() - lastPublish >= TRIAL_PUBLISH_RATE) {
      lastPublish += TRIAL_PUBLISH_RATE;
      publish_telemetry();
    }
    if (millis() - lastPublish >= FIVE_SECONDS) {
      lastPublish += FIVE_SECONDS;
      publish_telemetry();
    }
  }
}
