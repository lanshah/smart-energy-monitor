/*
 * display.h - 128x64 I2C OLED Update Routine
 * Smart Energy Monitoring and Appliance Control System | ACE6263
 *
 * Uses Adafruit_SSD1306 + Adafruit_GFX libraries.
 * Shows: active appliance, voltage, current, power, temperature,
 * humidity, and Blynk connection status on a single screen.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void updateOLED(Adafruit_SSD1306 &oled,
                 const char* activeAppliance,
                 float voltage, float current, float power,
                 float tempC, float humidity,
                 bool blynkConnected) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);

  oled.setCursor(0, 0);
  oled.print("Load: ");
  oled.println(activeAppliance);

  oled.setCursor(0, 12);
  oled.printf("V:%.2fV  I:%.3fA", voltage, current);

  oled.setCursor(0, 24);
  oled.printf("P:%.2fW", power);

  oled.setCursor(0, 36);
  oled.printf("T:%.1fC  H:%.1f%%", tempC, humidity);

  oled.setCursor(0, 48);
  oled.print("Blynk: ");
  oled.println(blynkConnected ? "CONNECTED" : "OFFLINE");

  oled.display();
}

#endif // DISPLAY_H
