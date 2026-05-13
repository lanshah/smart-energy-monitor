/*
 * led_status.h — RGB LED Strip Status Indicator
 * Smart Energy Monitoring System | ACE6263
 *
 * Colour coding:
 *   Green  → Normal operation  (power ≤ 70 % of threshold)
 *   Yellow → Moderate load     (70–100 % of threshold)
 *   Red    → Alert / Over threshold
 *   Blue   → Wi-Fi connecting / offline
 */

#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>
#include "config.h"

// LEDC channels
#define PWM_CH_RED   0
#define PWM_CH_GREEN 1
#define PWM_CH_BLUE  2

void initLED() {
  ledcSetup(PWM_CH_RED,   LED_PWM_FREQ, LED_PWM_RES);
  ledcSetup(PWM_CH_GREEN, LED_PWM_FREQ, LED_PWM_RES);
  ledcSetup(PWM_CH_BLUE,  LED_PWM_FREQ, LED_PWM_RES);

  ledcAttachPin(LED_RED_PIN,   PWM_CH_RED);
  ledcAttachPin(LED_GREEN_PIN, PWM_CH_GREEN);
  ledcAttachPin(LED_BLUE_PIN,  PWM_CH_BLUE);

  // Startup: blue
  ledcWrite(PWM_CH_RED,   0);
  ledcWrite(PWM_CH_GREEN, 0);
  ledcWrite(PWM_CH_BLUE, 200);
  Serial.println("[OK] RGB LED initialised.");
}

void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(PWM_CH_RED,   r);
  ledcWrite(PWM_CH_GREEN, g);
  ledcWrite(PWM_CH_BLUE,  b);
}

void updateLED(float powerW, float threshold) {
  float ratio = powerW / threshold;
  if (ratio >= 1.0f) {
    // RED — over threshold
    setLEDColor(255, 0, 0);
  } else if (ratio >= 0.7f) {
    // YELLOW — moderate load
    setLEDColor(255, 140, 0);
  } else {
    // GREEN — normal
    setLEDColor(0, 200, 0);
  }
}

void setLEDOffline() {
  setLEDColor(0, 0, 200);  // Blue = offline/connecting
}

#endif // LED_STATUS_H
