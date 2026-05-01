#include "trial_logic.h"
#include "config.h"
#include "persistence.h"
#include <M5StickCPlus2.h>

// External variables
extern int startBtnState, stopBtnState, resetBtnState, trialRunning, forceStop;
extern int buttonPushLatch, faderLatch, angleLatch, cableWrapProbeStowLatch, probeGoalLatch;
extern int faderValue, angleValue, probeGoalState, humanStart, humanAttempts;
extern int OP180_1_State, OP180_2_State;
extern int trialCounter, countStart, screenSelector, trialCompletedLatch, timeLeft;
extern unsigned long trialTime, startTime, now;
extern int display[4];
extern float accX, accY, accZ, startaccX, startaccY, startaccZ;
extern volatile unsigned long usecCount;
extern int stopBtnState_old;
extern Preferences myPrefs;

void turn_LED_off();
void turn_LED_on();
void resetCounter();

void check_trialStartStopLogic() {
  // Start Trial on M5 Button Press Check
  if (startBtnState == BUTTON_ON && trialRunning == 0 && stopBtnState == BUTTON_OFF && forceStop == 0) {
    delay(1);
    if (OP180_1_State == 1 || OP180_2_State == 1) {
      M5.Display.setCursor(5, 5);
      M5.Display.setTextColor(WHITE, RED);
      M5.Display.setTextSize(2);
      M5.Display.fillScreen(RED);
      M5.Display.printf("Unwind the cable \n from the wrap \n posts!");
      delay(1000);
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.fillScreen(BLACK);
    }
    if (faderValue > 20) {
      M5.Display.setCursor(5, 5);
      M5.Display.setTextColor(WHITE, RED);
      M5.Display.setTextSize(2);
      M5.Display.fillScreen(RED);
      M5.Display.printf("Move fader to left\n end stop!");
      delay(1000);
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.fillScreen(BLACK);
    }
    if (angleValue < 3500) {
      M5.Display.setCursor(5, 5);
      M5.Display.setTextColor(WHITE, RED);
      M5.Display.setTextSize(2);
      M5.Display.fillScreen(RED);
      M5.Display.printf("Close door!");
      delay(1000);
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.fillScreen(BLACK);
    }
    if (probeGoalState == BUTTON_ON) {
      M5.Display.setCursor(5, 5);
      M5.Display.setTextColor(WHITE, RED);
      M5.Display.setTextSize(2);
      M5.Display.fillScreen(RED);
      M5.Display.printf("Move probe plug to\n black port!");
      delay(1000);
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.fillScreen(BLACK);
    }
    if (startBtnState == BUTTON_ON && faderValue < 20 && angleValue > 3500 && probeGoalState == BUTTON_OFF && OP180_1_State == 0 && OP180_2_State == 0) {
      if (M5.BtnB.isPressed() == 1) {
        humanStart = 1;
        humanAttempts++;
      }
      M5.Display.fillScreen(BLUE);
      M5.Display.setTextColor(BLACK, BLUE);
      countStart = 1;
      usecCount = 0;
      startTime = now;
      trialRunning = 1;
      startaccX = accX;
      startaccY = accY;
      startaccZ = accZ;
      trialCounter++;
      Serial.printf("Writing values into NVM...");
      myPrefs.begin("task-board", false);
      myPrefs.putUInt("trialCounter", trialCounter);
      myPrefs.putUInt("humanAttempts", humanAttempts);
      myPrefs.end();
      Serial.printf("done!\n");
      Serial.printf("%d us Trial_Event: M5 Button pressed, Trial %d Started!\n", trialTime, trialCounter);
    }
    delay(1);
  }

  // Stop Trial on RED Button Press Check
  if (stopBtnState == BUTTON_ON && trialRunning > 0 && buttonPushLatch == 1 && faderLatch == 1 && angleLatch == 1 && cableWrapProbeStowLatch == 1 && probeGoalLatch == 1) {
    delay(1);
    if (stopBtnState == BUTTON_ON) {
      countStart = 0;
      turn_LED_off();
      Serial.printf("%d us Trial_Event: Red Button pressed, End of Successful Trial %d! Congrats!\n", usecCount, trialCounter);
      trialCompletedLatch = 1;
    }
    delay(1);
  }

  if (stopBtnState == 0 && stopBtnState != stopBtnState_old) {
    Serial.printf("%d us Task_Board_Event: Red Push Button Pressed\n", usecCount);
  }
  if (stopBtnState == 1 && stopBtnState != stopBtnState_old) {
    Serial.printf("%d us Task_Board_Event: Red Push Button Released\n", usecCount);
  }
  stopBtnState_old = stopBtnState;

  // FORCE Stop Trial on RED Button Press Check
  if (stopBtnState == BUTTON_ON && trialRunning > 0 && M5.BtnA.isPressed() == 1) {
    delay(1);
    if (stopBtnState == BUTTON_ON) {
      countStart = 0;
      turn_LED_off();
      Serial.printf("Trial_Event: Trial %d Aborted!\n", trialCounter);
      trialRunning = 0;
      M5.Display.fillScreen(BLACK);
      M5.Display.setTextColor(WHITE, BLACK);
      forceStop = 1;
    }
    delay(1);
  }

  // Reset the trial counter in NVM
  if (trialRunning == 0 && stopBtnState == BUTTON_ON && buttonPushLatch == 0 && M5.BtnB.isPressed() == 1) {
    resetCounter();
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(5, 5);
    M5.Display.printf("Trial Counters Reset!");
    delay(1000);
    M5.Display.fillScreen(BLACK);
  }

  // Time Limit Check
  timeLeft = round(TIMELIMIT - usecCount / 1000000);
  if (trialRunning == 1 && timeLeft <= 0) {
    delay(1);
    countStart = 0;
    Serial.printf("%d us Trial_Event: Trial Time Limit %d reached! Time's Up! \n", usecCount, TIMELIMIT);
    turn_LED_off();
    for (int i = 0; i < 3; i++) {
      M5.Display.fillScreen(RED);
      M5.Display.setCursor(5, 5);
      M5.Display.setTextSize(3);
      M5.Display.setTextColor(BLACK);
      M5.Display.printf("TIME IS UP!\nTRIAL OVER!");
      delay(200);
      M5.Display.fillScreen(BLACK);
      delay(200);
    }
    delay(1);
  }

  // Time Count Start
  if (countStart == 1 && trialRunning == 0) {
    startTime = now;
    trialRunning = 1;
  }

  // Time Count Stop
  if (countStart == 0 && trialRunning > 0) {
    trialRunning = 0;
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
  }

  // Trial Reset Check
  if (resetBtnState == BUTTON_ON && trialRunning == 0 && trialTime != 0) {
    Serial.println("Trial Reset Button pressed");
    forceStop = 0;
    usecCount = 0;
    buttonPushLatch = 0;
    faderLatch = 0;
    angleLatch = 0;
    cableWrapProbeStowLatch = 0;
    probeGoalLatch = 0;
    trialCompletedLatch = 0;
    trialTime = 0;
    display[0] = 0;
    display[1] = 0;
    display[2] = 0;
    turn_LED_off();
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
  }
}
