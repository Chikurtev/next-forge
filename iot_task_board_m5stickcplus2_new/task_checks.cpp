#include "task_checks.h"
#include "config.h"
#include <M5StickCPlus2.h>

// External variables
extern int buttonPushState, buttonPushState_old, trialRunning, buttonPushLatch, screenSelector;
extern int faderValue, faderValue_old, faderLatch, faderLatch2, faderGoal2;
extern int probeGoalState, probeGoalState_old, probeStartState, probeStartState_old, probeGoalLatch;
extern int angleValue, angleValue_old, angleLatch, angleDoorLatch;
extern int OP180_1_State, OP180_1_State_old, OP180_2_State, OP180_2_State_old, cableWrapLatch, cableWrapProbeStowLatch;
extern int stopBtnState;
extern unsigned long trialTime, usecCount;
extern int TS_button, TS_fader_mid, TS_fader, TS_probeGoal, TS_angle, TS_angle_door, TS_cableWrap, TS_cableWrapProbeStow;
extern int ptsCollected;

void blink_LED();

bool check_blue_button() {
  if (buttonPushState == 0 && buttonPushState != buttonPushState_old) {
    Serial.printf("%d us Task_Board_Event: Blue Push Button Pressed\n", usecCount);
  }
  if (buttonPushState == 1 && buttonPushState != buttonPushState_old) {
    Serial.printf("%d us Task_Board_Event: Blue Push Button Released\n", usecCount);
  }
  buttonPushState_old = buttonPushState;

  if (buttonPushState == BUTTON_ON && trialRunning == 1 && buttonPushLatch == 0) {
    buttonPushLatch = 1;
    TS_button = trialTime;
    ptsCollected = ptsCollected + PTS_BUTTON;
    blink_LED();
    Serial.printf("%d us Trial_Event: Button pushed!\n", usecCount);
    StickCP2.Display.fillScreen(BLUE);
    screenSelector = 2;
  }
  return buttonPushLatch;
}

bool check_slider() {
  faderValue_old = faderValue;

  if (faderValue > FADERSP - FADERTOLERANCE && faderValue < FADERSP + FADERTOLERANCE && trialRunning == 1 && faderLatch2 == 0 && buttonPushLatch == 1) {
    faderLatch2 = 1;
    TS_fader_mid = trialTime;
    Serial.printf("%d us Trial_Event: Fader SP1 Matched! %d\n", usecCount, FADERSP);
    StickCP2.Display.fillScreen(YELLOW);
    delay(50);
    StickCP2.Display.fillScreen(BLUE);

    int coin = random(10, 100);
    if (coin >= 50) {
      faderGoal2 = FADERSP - FADERSP2 * coin / 100;
    } else {
      faderGoal2 = FADERSP + FADERSP2 * coin / 100;
    }
  }

  if (faderValue > faderGoal2 - FADERTOLERANCE && faderValue < faderGoal2 + FADERTOLERANCE && faderLatch == 0 && faderLatch2 == 1) {
    faderLatch = 1;
    TS_fader = trialTime;
    ptsCollected = ptsCollected + PTS_FADER;
    blink_LED();
    Serial.printf("%d us Trial_Event: Fader SP2 Matched! %d \n", usecCount, faderGoal2);
    StickCP2.Display.fillScreen(YELLOW);
    delay(50);
    StickCP2.Display.fillScreen(BLUE);
    screenSelector = 0;
  }
  return faderLatch;
}

bool check_probe_plug() {
  if (probeGoalState == 0 && probeGoalState != probeGoalState_old) {
    Serial.printf("%d us Task_Board_Event: Probe plug is plugged in and Probe Tip is in holder\n", usecCount);
  }
  if (probeGoalState == 1 && probeGoalState != probeGoalState_old) {
    Serial.printf("%d us Task_Board_Event: Probe plug is not plugged in\n", usecCount);
  }
  probeGoalState_old = probeGoalState;

  if (probeStartState == 0 && probeStartState != probeStartState_old) {
    Serial.printf("%d us Task_Board_Event: Probe is touching goal circuit\n", usecCount);
  }
  if (probeStartState == 1 && probeStartState != probeStartState_old) {
    Serial.printf("%d us Task_Board_Event: Probe is not touching goal circuit\n", usecCount);
  }
  probeStartState_old = probeStartState;

  if (probeGoalState == BUTTON_ON && trialRunning == 1 && probeGoalLatch == 0) {
    probeGoalLatch = 1;
    TS_probeGoal = trialTime;
    ptsCollected = ptsCollected + PTS_PROBEINSERT;
    blink_LED();
    Serial.printf("%d us Trial_Event: Probe Plug inserted!\n", usecCount);
    StickCP2.Display.fillScreen(BLUE);
  }
  return probeGoalLatch;
}

bool check_door() {
  angleValue_old = angleValue;

  if (angleValue < ANGLESP && trialRunning == 1 && angleDoorLatch == 0) {
    angleDoorLatch = 1;
    TS_angle_door = trialTime;
    Serial.printf("%d us Task_Board_Event: Door Angle achieved!\n", usecCount);
  }

  if (angleValue < ANGLESP && trialRunning == 1 && angleLatch == 0 && probeStartState == BUTTON_ON) {
    angleLatch = 1;
    TS_angle = trialTime;
    Serial.printf("%d us Trial_Event: Terminal Block Circuit Probed!\n", usecCount);
    ptsCollected = ptsCollected + PTS_CIRCUIT_PROBE;
    blink_LED();
    StickCP2.Display.fillScreen(BLUE);
  }
  return angleDoorLatch;
}

bool check_cable_posts() {
  if (OP180_1_State == BUTTON_OFF && OP180_2_State == BUTTON_OFF && trialRunning == 1 && cableWrapLatch == 0) {
    cableWrapLatch = 1;
    TS_cableWrap = trialTime;
    Serial.printf("%d us Trial_Event: Cable successfully wrapped!\n", usecCount);
  }
  return cableWrapLatch;
}

bool check_probe_tip_holder() {
  if (OP180_1_State == BUTTON_OFF && OP180_2_State == BUTTON_OFF && trialRunning == 1 && cableWrapProbeStowLatch == 0 && angleLatch == 1 && probeGoalState == BUTTON_ON) {
    cableWrapProbeStowLatch = 1;
    TS_cableWrapProbeStow = trialTime;
    Serial.printf("%d us Trial_Event: Cable successfully wrapped AND Probe Tip Stowed!\n", usecCount);
    ptsCollected = ptsCollected + PTS_CABLEWRAP;
    blink_LED();
    StickCP2.Display.fillScreen(BLUE);
  }
  return cableWrapProbeStowLatch;
}
