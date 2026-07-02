/*
 * relay_ctrl.h - Four-Channel Relay Control with Exclusive Appliance Mode
 * Smart Energy Monitoring and Appliance Control System | ACE6263
 *
 * The relay module is ACTIVE-LOW:
 *   LOW  → relay ON  (appliance powered)
 *   HIGH → relay OFF (appliance de-powered)
 *
 * EXCLUSIVE MODE:
 *   Only one appliance may be active at a time, because the system uses a
 *   single ACS712 current sensor placed in series with the common supply
 *   feeding all four relay COM terminals. If more than one appliance were
 *   powered simultaneously, the measured current would be the sum of all
 *   active loads and could not be attributed to a single appliance.
 *
 *   Whenever a new appliance is switched ON, any currently active appliance
 *   is automatically switched OFF first.
 */

#ifndef RELAY_CTRL_H
#define RELAY_CTRL_H

#include <Arduino.h>
#include "config.h"

static const uint8_t RELAY_PINS[4] = {
  RELAY_PUMP_PIN, RELAY_LED_PIN, RELAY_SEVENSEG_PIN, RELAY_FAN_PIN
};

static const char* APPLIANCE_NAMES[4] = {
  "AIR PUMP", "LED", "SEVEN-SEGMENT", "USB FAN"
};

// Index (0-3) of the currently active appliance, or APPLIANCE_NONE if all off
static int gActiveAppliance = APPLIANCE_NONE;

// ─────────────────────────────────────────────────────────────
// initRelays()
//   Configures all relay GPIOs as OUTPUT and switches every
//   channel OFF (all appliances start de-energised).
// ─────────────────────────────────────────────────────────────
void initRelays() {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], HIGH);  // HIGH = OFF (active-low module)
  }
  gActiveAppliance = APPLIANCE_NONE;
  Serial.println("[OK] Relays initialised - all appliances OFF.");
}

// ─────────────────────────────────────────────────────────────
// turnOffAllRelays()
//   Forces every relay channel OFF. Used internally before
//   activating a new appliance, and available for a safe-state
//   reset.
// ─────────────────────────────────────────────────────────────
void turnOffAllRelays() {
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(RELAY_PINS[i], HIGH);
  }
  gActiveAppliance = APPLIANCE_NONE;
}

// ─────────────────────────────────────────────────────────────
// setApplianceExclusive(index, turnOn)
//   Applies exclusive-mode logic:
//     - If turnOn == true:  switch off whichever appliance is
//       currently active (if different), then switch on `index`.
//     - If turnOn == false: switch off `index` only if it is the
//       currently active appliance.
//
//   @param index   Appliance index 0-3 (APPLIANCE_PUMP .. APPLIANCE_FAN)
//   @param turnOn  Desired state requested by the user (via Blynk)
// ─────────────────────────────────────────────────────────────
void setApplianceExclusive(int index, bool turnOn) {
  if (index < 0 || index > 3) return;

  if (turnOn) {
    if (gActiveAppliance != APPLIANCE_NONE && gActiveAppliance != index) {
      // Turn off the previously active appliance first
      digitalWrite(RELAY_PINS[gActiveAppliance], HIGH);
      Serial.printf("[RELAY] %s turned OFF (exclusive mode)\n",
                    APPLIANCE_NAMES[gActiveAppliance]);
    }
    digitalWrite(RELAY_PINS[index], LOW);   // ON (active-low)
    gActiveAppliance = index;
    Serial.printf("[RELAY] %s turned ON\n", APPLIANCE_NAMES[index]);
  } else {
    if (gActiveAppliance == index) {
      digitalWrite(RELAY_PINS[index], HIGH);  // OFF
      gActiveAppliance = APPLIANCE_NONE;
      Serial.printf("[RELAY] %s turned OFF\n", APPLIANCE_NAMES[index]);
    }
    // If a different appliance is active, an OFF command for `index`
    // has no effect, since it was already off.
  }
}

// ─────────────────────────────────────────────────────────────
// getActiveApplianceName()
//   Returns a human-readable name of the active appliance, or
//   "NONE" if all appliances are off. Used for the OLED display.
// ─────────────────────────────────────────────────────────────
const char* getActiveApplianceName() {
  if (gActiveAppliance == APPLIANCE_NONE) return "NONE";
  return APPLIANCE_NAMES[gActiveAppliance];
}

// ─────────────────────────────────────────────────────────────
// isApplianceOn(index)
//   Returns true if the given appliance index is currently active.
//   Used to send synchronized switch states back to Blynk.
// ─────────────────────────────────────────────────────────────
bool isApplianceOn(int index) {
  return (gActiveAppliance == index);
}

#endif // RELAY_CTRL_H
