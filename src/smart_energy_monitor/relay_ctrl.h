/*
 * relay_ctrl.h — 4-Channel Relay Module Control
 * Smart Energy Monitoring System | ACE6263
 *
 * The relay module is ACTIVE-LOW: writing LOW to the GPIO energises the coil
 * and closes the normally-open (NO) contact. Writing HIGH de-energises it.
 */

#ifndef RELAY_CTRL_H
#define RELAY_CTRL_H

#include <Arduino.h>
#include "config.h"

static const uint8_t RELAY_PINS[4] = {
  RELAY_PIN_0, RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3
};

// ─────────────────────────────────────────────────────────────
// initRelays()
//   Configures all relay GPIO pins as OUTPUT and ensures all
//   relays start in the OFF (de-energised) state.
// ─────────────────────────────────────────────────────────────
void initRelays() {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], HIGH);  // HIGH = OFF for active-low module
  }
  Serial.println("[OK] Relays initialised (all OFF).");
}

// ─────────────────────────────────────────────────────────────
// setRelay(index, state)
//   @param index  Relay channel 0–3
//   @param state  true = ON (energise coil), false = OFF
// ─────────────────────────────────────────────────────────────
void setRelay(uint8_t index, bool state) {
  if (index >= 4) return;
  // Active-low: ON → LOW, OFF → HIGH
  digitalWrite(RELAY_PINS[index], state ? LOW : HIGH);
  Serial.printf("[RELAY] CH%u → %s\n", index, state ? "ON" : "OFF");
}

// ─────────────────────────────────────────────────────────────
// getRelayState(index)
//   Returns true if the relay coil is energised (channel ON).
// ─────────────────────────────────────────────────────────────
bool getRelayState(uint8_t index) {
  if (index >= 4) return false;
  return (digitalRead(RELAY_PINS[index]) == LOW);
}

// ─────────────────────────────────────────────────────────────
// allRelaysOff()  — Emergency / safe-state helper
// ─────────────────────────────────────────────────────────────
void allRelaysOff() {
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(RELAY_PINS[i], HIGH);
  }
  Serial.println("[RELAY] All channels set to OFF.");
}

#endif // RELAY_CTRL_H
