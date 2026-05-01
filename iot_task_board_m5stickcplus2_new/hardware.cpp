#include "hardware.h"

void turn_LED_on() {
  digitalWrite(LED_PIN, HIGH);
}

void turn_LED_off() {
  digitalWrite(LED_PIN, LOW);
}

void blink_LED() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}
