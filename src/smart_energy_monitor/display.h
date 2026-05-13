/*
 * display.h — 16×2 I2C LCD Update Routines
 * Smart Energy Monitoring System | ACE6263
 *
 * Alternates between two screens every call:
 *   Screen A: Voltage / Current / Power / Energy
 *   Screen B: Temperature / Humidity / Power / Energy
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

static bool _lcdScreen = false;  // false = Screen A, true = Screen B

void updateLCD(LiquidCrystal_I2C &lcd,
               float vRMS, float iRMS, float powerW, float energyKWh,
               float tempC, float humidity) {
  lcd.clear();
  if (!_lcdScreen) {
    // ── Screen A ──────────────────────────────────────────────
    // Row 0:  "230.1V  0.440A"
    // Row 1:  "100.5W  0.031kWh"
    lcd.setCursor(0, 0);
    lcd.printf("%.1fV  %.3fA", vRMS, iRMS);
    lcd.setCursor(0, 1);
    lcd.printf("%.1fW  %.4fkWh", powerW, energyKWh);
  } else {
    // ── Screen B ──────────────────────────────────────────────
    // Row 0:  "T:28.3C  H:65.2%"
    // Row 1:  "P:100.5W [ON]"
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", tempC, humidity);
    lcd.setCursor(0, 1);
    lcd.printf("P:%.1fW %.4fkWh", powerW, energyKWh);
  }
  _lcdScreen = !_lcdScreen;
}

#endif // DISPLAY_H
