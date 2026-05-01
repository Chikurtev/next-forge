#include "persistence.h"

// External variables
extern Preferences myPrefs;
extern int trialCounter, humanAttempts;

void resetCounter() {
  myPrefs.begin("task-board", false);
  myPrefs.clear();
  myPrefs.end();
  trialCounter = 0;
  humanAttempts = 0;
  Serial.printf("Trial Counter value reset!\n");
}
