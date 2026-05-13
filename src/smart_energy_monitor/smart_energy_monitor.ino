/*
 * ============================================================
 *  Smart Energy Monitoring System
 *  ACE6263 — Embedded IoT Systems | Trimester March/April 2026
 *  SDG 7 — Affordable and Clean Energy
 * ============================================================
 *
 *  Hardware:
 *    - ESP32 Dev Board
 *    - ZMPT101B B25 Voltage Sensor Module   (ADC via ADS1115 ch0)
 *    - ACS712 30A Current Sensor Module     (ADC via ADS1115 ch1)
 *    - DHT22 Temperature & Humidity Sensor  (GPIO 4)
 *    - ADS1115 16-bit ADC (I2C 0x48)
 *    - 4-Channel Relay Module               (GPIO 16-19)
 *    - 16×2 I2C LCD Display                 (I2C 0x27)
 *    - RGB LED Strip                        (PWM GPIO 21, 22, 23)
 *
 *  IoT Platform: Blynk (cloud.blynk.cc)
 *
 *  Authors  : Group TBD — ACE6263
 *  Version  : 1.0.0
 *  Date     : 2026-06-01
 * ============================================================
 */

// ─── Library Includes ────────────────────────────────────────
#define BLYNK_TEMPLATE_ID   "TMPLxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "SmartEnergyMonitor"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include "energy_calc.h"
#include "relay_ctrl.h"
#include "display.h"
#include "led_status.h"

// ─── Object Instances ─────────────────────────────────────────
Adafruit_ADS1115 ads;
DHT dht(DHT22_PIN, DHT22);
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
BlynkTimer timer;

// ─── Global State ─────────────────────────────────────────────
float gVoltageRMS   = 0.0f;
float gCurrentRMS   = 0.0f;
float gPowerW       = 0.0f;
float gEnergyKWh    = 0.0f;
float gTemperature  = 0.0f;
float gHumidity     = 0.0f;
float gPowerFactor  = 1.0f;

bool  relayState[4] = {false, false, false, false};

unsigned long lastEnergyUpdateMs = 0;

// ─── Blynk Virtual Pin Handlers (Relay Control) ───────────────
BLYNK_WRITE(V10) { relayState[0] = param.asInt(); setRelay(0, relayState[0]); }
BLYNK_WRITE(V11) { relayState[1] = param.asInt(); setRelay(1, relayState[1]); }
BLYNK_WRITE(V12) { relayState[2] = param.asInt(); setRelay(2, relayState[2]); }
BLYNK_WRITE(V13) { relayState[3] = param.asInt(); setRelay(3, relayState[3]); }

// ─── Blynk Connected Callback ─────────────────────────────────
BLYNK_CONNECTED() {
  Blynk.syncAll();
  Serial.println("[Blynk] Connected & synced.");
}

// ─── Sensor Acquisition & Publish ─────────────────────────────
void readAndPublish() {
  // --- Voltage (ZMPT101B B25 via ADS1115 ch0) ---
  gVoltageRMS = measureVoltageRMS(ads, ADS_VOLTAGE_CH, ZMPT101B_CALIBRATION_FACTOR);

  // --- Current (ACS712 via ADS1115 ch1) ---
  gCurrentRMS = measureCurrentRMS(ads, ADS_CURRENT_CH, ACS712_SENSITIVITY_MV_PER_A,
                                  ACS712_MIDPOINT_MV, ADS_VREF_MV);

  // --- Power & Energy ---
  gPowerW     = gVoltageRMS * gCurrentRMS * gPowerFactor;

  unsigned long now = millis();
  float deltaHours  = (now - lastEnergyUpdateMs) / 3600000.0f;
  gEnergyKWh       += (gPowerW / 1000.0f) * deltaHours;
  lastEnergyUpdateMs = now;

  // --- DHT22 ---
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    gTemperature = t;
    gHumidity    = h;
  }

  // --- LCD Update ---
  updateLCD(lcd, gVoltageRMS, gCurrentRMS, gPowerW, gEnergyKWh, gTemperature, gHumidity);

  // --- RGB LED Status ---
  updateLED(gPowerW, POWER_ALERT_THRESHOLD_W);

  // --- Blynk Publish ---
  Blynk.virtualWrite(V0, gVoltageRMS);
  Blynk.virtualWrite(V1, gCurrentRMS);
  Blynk.virtualWrite(V2, gPowerW);
  Blynk.virtualWrite(V3, gEnergyKWh);
  Blynk.virtualWrite(V4, gTemperature);
  Blynk.virtualWrite(V5, gHumidity);
  Blynk.virtualWrite(V6, gPowerFactor);

  // --- Threshold Alert ---
  if (gPowerW > POWER_ALERT_THRESHOLD_W) {
    Blynk.logEvent("high_power_alert",
      String("Power exceeded threshold: ") + String(gPowerW, 1) + " W");
  }

  // --- Serial Debug ---
  Serial.printf("[DATA] V=%.2fV  I=%.3fA  P=%.2fW  E=%.4fkWh  T=%.1f°C  H=%.1f%%\n",
                gVoltageRMS, gCurrentRMS, gPowerW, gEnergyKWh, gTemperature, gHumidity);
}

// ─── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Smart Energy Monitor — Booting ===");

  // Relay initialisation
  initRelays();

  // I2C peripherals
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // ADS1115
  if (!ads.begin(ADS1115_ADDR)) {
    Serial.println("[ERROR] ADS1115 not found!");
    while (1);
  }
  ads.setGain(GAIN_ONE);   // ±4.096 V range → 0.125 mV/bit
  ads.setDataRate(RATE_ADS1115_860SPS);
  Serial.println("[OK] ADS1115 initialised.");

  // DHT22
  dht.begin();
  Serial.println("[OK] DHT22 initialised.");

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("SmartEnergy v1.0");
  lcd.setCursor(0, 1); lcd.print("Connecting...");
  Serial.println("[OK] LCD initialised.");

  // RGB LED
  initLED();

  // Blynk + Wi-Fi
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("WiFi Connected!");
  delay(1000);

  // Timer: every 2 000 ms
  lastEnergyUpdateMs = millis();
  timer.setInterval(SAMPLE_INTERVAL_MS, readAndPublish);

  Serial.println("[OK] Setup complete. Entering main loop.");
}

// ─── Loop ─────────────────────────────────────────────────────
void loop() {
  Blynk.run();
  timer.run();
}
