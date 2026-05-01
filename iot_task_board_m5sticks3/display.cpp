#include "display.h"
#include "config.h"
#include <M5Unified.h>
#include <WiFi.h>

// External variables declared in main
extern int screenSelector;
extern int buttonPushLatch, faderLatch, probeGoalLatch, angleLatch, cableWrapProbeStowLatch, trialCompletedLatch;
extern int humanStart, trialRunning;
extern int faderValue, angleValue, faderLatch2, faderGoal2;
extern unsigned long trialTime;
extern int TS_button, TS_fader, TS_probeGoal, TS_angle, TS_cableWrapProbeStow, TS_finalTrialTime;
extern int ptsCollected, trialCounter, humanAttempts, wifiEnabled;
extern int OP180_1_State, OP180_2_State;
extern float cumForce;
extern int display[4];

void home_screen() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  if (buttonPushLatch) {
    if (humanStart) {
      M5.Display.fillCircle(10, 10, 8, RED);
    } else {
      M5.Display.fillCircle(10, 10, 8, GREEN);
    }
  }
  M5.Display.drawCircle(40, 10, 10, WHITE);
  if (faderLatch) {
    if (humanStart) {
      M5.Display.fillCircle(40, 10, 8, RED);
    } else {
      M5.Display.fillCircle(40, 10, 8, GREEN);
    }
  }
  M5.Display.drawCircle(70, 10, 10, WHITE);
  if (probeGoalLatch) {
    if (humanStart) {
      M5.Display.fillCircle(70, 10, 8, RED);
    } else {
      M5.Display.fillCircle(70, 10, 8, GREEN);
    }
  }
  M5.Display.drawCircle(100, 10, 10, WHITE);
  if (angleLatch) {
    if (humanStart) {
      M5.Display.fillCircle(100, 10, 8, RED);
    } else {
      M5.Display.fillCircle(100, 10, 8, GREEN);
    }
  }
  M5.Display.drawCircle(130, 10, 10, WHITE);
  if (cableWrapProbeStowLatch) {
    if (humanStart) {
      M5.Display.fillCircle(130, 10, 8, RED);
    } else {
      M5.Display.fillCircle(130, 10, 8, GREEN);
    }
  }
  M5.Display.drawCircle(160, 10, 10, WHITE);
  if (trialCompletedLatch) {
    if (humanStart) {
      M5.Display.fillCircle(160, 10, 8, RED);
    } else {
      M5.Display.fillCircle(160, 10, 8, GREEN);
    }
  }
  M5.Display.setCursor(5, 25);
  M5.Display.setTextSize(1);
  M5.Display.printf("%0.1f %0.1f %0.1f %0.1f %0.1f %0.1f\n", (float)TS_button / 1000.0, (float)TS_fader / 1000.0, (float)TS_probeGoal / 1000.0, (float)TS_angle / 1000.0, (float)TS_cableWrapProbeStow / 1000.0, (float)TS_finalTrialTime / 1000.0);
  M5.Display.printf(" Smart Task Board");
  M5.Display.printf(" v%s \n", FW_VERSION.c_str());
  M5.Display.printf(" Wifi On:%d Status:%d Batt:%0.1fV Lvl:%d\n", wifiEnabled, WiFi.status(), (float)M5.Power.getBatteryVoltage() / 1000, (int)M5.Power.getBatteryLevel());
  M5.Display.printf(" Token: %s\n", TOKEN.c_str());
  M5.Display.printf(" PROTOCOL:%s\n", PROTOCOL_ID);
  M5.Display.printf(" Total Attempts:%d Human Attempts:%d\n", trialCounter, humanAttempts);
  M5.Display.printf(" Points:%d Interaction:%0.2f\n", ptsCollected, cumForce);
  M5.Display.printf(" Trial Time (secs):\n");
  M5.Display.setTextSize(3);
  M5.Display.printf(" %0.2f\n ", (float)trialTime / 1000.0);
}

void develop_screen() {
  // Development screen - removed for now
}

void screen2() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  M5.Display.fillCircle(10, 10, 3, YELLOW);
  M5.Display.drawCircle(40, 10, 10, WHITE);
  M5.Display.drawCircle(70, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 10, WHITE);
  M5.Display.drawCircle(130, 10, 10, WHITE);
  M5.Display.drawCircle(160, 10, 10, WHITE);
  M5.Display.setCursor(5, 15);
  M5.Display.setTextSize(1);
  M5.Display.printf("\n ");
  M5.Display.printf("Subtask:1/6\n ");
  M5.Display.printf("PROTOCOL: %s\n ", PROTOCOL_ID);
  M5.Display.printf("Find Board, Press Button\n ");
  M5.Display.setTextSize(3);
  M5.Display.printf("%02dm:%02ds:%03dms\n", display[0], display[1], display[2]);
}

void screen3() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  M5.Display.drawCircle(40, 10, 10, WHITE);
  M5.Display.drawCircle(70, 10, 9, YELLOW);
  M5.Display.fillCircle(70, 10, 3, YELLOW);
  M5.Display.drawCircle(70, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 10, WHITE);
  M5.Display.drawCircle(130, 10, 10, WHITE);
  M5.Display.drawCircle(160, 10, 10, WHITE);
  M5.Display.setCursor(5, 15);
  M5.Display.setTextSize(1);
  M5.Display.printf("\n ");
  M5.Display.printf("Subtask:3/6\n ");
  M5.Display.printf("PROTOCOL: %s\n ", PROTOCOL_ID);
  M5.Display.printf("Insert Probe Cable Plug\n ");
  M5.Display.setTextSize(3);
  M5.Display.printf("%02dm:%02ds:%03dms\n", display[0], display[1], display[2]);
}

void screen4() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  if (buttonPushLatch) {
    if (humanStart) {
      M5.Display.fillCircle(10, 10, 8, RED);
    } else {
      M5.Display.fillCircle(10, 10, 8, GREEN);
    }
  }
  M5.Display.drawCircle(40, 10, 10, WHITE);
  M5.Display.fillCircle(40, 10, 3, YELLOW);
  M5.Display.drawCircle(70, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 10, WHITE);
  M5.Display.drawCircle(130, 10, 10, WHITE);
  M5.Display.drawCircle(160, 10, 10, WHITE);
  M5.Display.setCursor(5, 22);
  M5.Display.setTextSize(1);
  M5.Display.printf("%0.1f %0.1f %0.1f %0.1f %0.1f %0.1f\n", (float)TS_button / 1000.0, (float)TS_fader / 1000.0, (float)TS_probeGoal / 1000.0, (float)TS_angle / 1000.0, (float)TS_cableWrapProbeStow / 1000.0, (float)trialTime / 1000.0);
  M5.Display.printf("Move Slider to align arrows\n");
  M5.Display.printf(" Trial Time (secs):\n");
  M5.Display.setTextSize(3);
  M5.Display.printf("%0.2f\n ", (float)trialTime / 1000.0);
  M5.Display.setTextSize(1);
  int x_offset = map(faderValue, 0, 4000, 10, 210);
  int x_goal = map(FADERSP, 0, 4000, 10, 210);
  int x_goal2 = map(faderGoal2, 0, 4000, 10, 210);
  M5.Display.fillRect(0, 80, 240, 25, BLACK);
  M5.Display.fillTriangle(0 + x_offset, 80, 20 + x_offset, 80, 10 + x_offset, 100, RED);
  M5.Display.fillTriangle(0 + x_goal, 120, 20 + x_goal, 120, 10 + x_goal, 100, YELLOW);
  if (faderLatch2 == 1) {
    M5.Display.fillTriangle(0 + x_goal2, 120, 20 + x_goal2, 120, 10 + x_goal2, 100, GREEN);
  }
}

void screen5() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  M5.Display.drawCircle(40, 10, 10, WHITE);
  M5.Display.drawCircle(70, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 9, YELLOW);
  M5.Display.fillCircle(100, 10, 3, YELLOW);
  M5.Display.drawCircle(130, 10, 10, WHITE);
  M5.Display.drawCircle(160, 10, 10, WHITE);
  M5.Display.setCursor(5, 15);
  M5.Display.setTextSize(1);
  M5.Display.printf("\n ");
  M5.Display.printf("Subtask:4/6\n ");
  M5.Display.printf("PROTOCOL: %s\n ", PROTOCOL_ID);
  M5.Display.printf("Open door, probe circuit\n ");
  M5.Display.setTextSize(3);
  M5.Display.printf("%02dm:%02ds:%03dms\n", display[0], display[1], display[2]);
}

void screen6() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  M5.Display.drawCircle(40, 10, 10, WHITE);
  M5.Display.drawCircle(70, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 10, WHITE);
  M5.Display.drawCircle(130, 10, 10, WHITE);
  M5.Display.drawCircle(130, 10, 9, YELLOW);
  M5.Display.fillCircle(130, 10, 3, YELLOW);
  M5.Display.drawCircle(160, 10, 10, WHITE);
  M5.Display.setCursor(5, 15);
  M5.Display.setTextSize(1);
  M5.Display.printf("\n ");
  M5.Display.printf("Subtask:5/6\n ");
  M5.Display.printf("PROTOCOL: %s\n ", PROTOCOL_ID);
  M5.Display.printf("Wrap probe cable and plug in probe\n ");
  M5.Display.setTextSize(3);
  M5.Display.printf("%02dm:%02ds:%03dms\n", display[0], display[1], display[2]);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(50, 120);
  M5.Display.printf("Post Near");
  M5.Display.drawCircle(70, 100, 10, WHITE);
  if (OP180_1_State == 1) {
    M5.Display.fillCircle(70, 100, 8, GREEN);
  } else {
    M5.Display.fillCircle(70, 100, 8, BLACK);
  }
  M5.Display.setCursor(140, 120);
  M5.Display.printf("Post Far");
  M5.Display.drawCircle(160, 100, 10, WHITE);
  if (OP180_2_State == 1) {
    M5.Display.fillCircle(160, 100, 8, GREEN);
  } else {
    M5.Display.fillCircle(160, 100, 8, BLACK);
  }
}

void screen7() {
  M5.Display.drawCircle(10, 10, 10, WHITE);
  M5.Display.drawCircle(40, 10, 10, WHITE);
  M5.Display.drawCircle(70, 10, 10, WHITE);
  M5.Display.drawCircle(100, 10, 10, WHITE);
  M5.Display.drawCircle(130, 10, 10, WHITE);
  M5.Display.drawCircle(160, 10, 10, WHITE);
  M5.Display.drawCircle(160, 10, 9, YELLOW);
  M5.Display.fillCircle(160, 10, 3, YELLOW);
  M5.Display.setCursor(5, 15);
  M5.Display.setTextSize(1);
  M5.Display.printf("\n ");
  M5.Display.printf("Subtask:6/6\n ");
  M5.Display.printf("PROTOCOL: %s\n ", PROTOCOL_ID);
  M5.Display.printf("Press trial stop button\n ");
  M5.Display.setTextSize(3);
  M5.Display.printf("%02dm:%02ds:%03dms\n", display[0], display[1], display[2]);
}

void update_screen() {
  static int lastScreenSelector = -1;
  if (screenSelector != lastScreenSelector) {
    lastScreenSelector = screenSelector;
    switch (screenSelector) {
      case 0:  // Idle
        home_screen();
        break;
      case 1:
        screen2();
        break;
      case 2:
        screen4();
        break;
      case 3:
        screen3();
        break;
      case 4:
        screen5();
        break;
      case 5:
        screen6();
        break;
      case 6:
        screen7();
        break;
      case 7:
        develop_screen();
        break;
      default:
        Serial.print("No code is available");
        break;
    }
  }
}
