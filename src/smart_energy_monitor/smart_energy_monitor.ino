/*
 * ============================================================
 *  Smart Energy Monitoring and Appliance Control System
 *  Using ESP32 and Blynk
 *  ACE6263 - Embedded IoT Systems | Trimester March/April 2026
 * ============================================================
 *
 *  IMPORTANT SAFETY NOTE:
 *  This system is designed ONLY for low-voltage DC testing and
 *  must NOT be connected directly to household AC mains electricity.
 *
 *  Hardware:
 *    - ESP32 Dev Board
 *    - B25 0-25V DC Voltage Sensor        (GPIO35)
 *    - ACS712 30A Current Sensor          (GPIO34)
 *    - DHT22 Temperature & Humidity       (GPIO32)
 *    - 128x64 I2C OLED Display            (SDA=21, SCL=22)
 *    - 4-Channel Relay Module (active-LOW)
 *        CH1 GPIO26 → Air pump
 *        CH2 GPIO27 → LED
 *        CH3 GPIO25 → Seven-segment (1 segment)
 *        CH4 GPIO33 → USB fan
 *
 *  IoT Platform: Blynk (cloud.blynk.cc)
 *
 *  Authors  : Group TBD - ACE6263
 *  Version  : 2.0.0
 *  Date     : 2026-07-01
 * ============================================================
 */

// ─── Blynk Template Credentials ───────────────────────────────
#define BLYNK_TEMPLATE_ID   "TMPLxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "SmartApplianceMonitor"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "energy_calc.h"
#include "relay_ctrl.h"
#include "display.h"

// ─── Object Instances ──────────────────────────────────────────
DHT dht(DHT22_PIN, DHT22);
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN);
BlynkTimer timer;

// ─── Global State ──────────────────────────────────────────────
float gVoltage     = 0.0f;
float gCurrent      = 0.0f;
float gPower         = 0.0f;
float gTemperature    = 0.0f;
float gHumidity        = 0.0f;
bool  gBlynkConnected  = false;

// ─── Blynk Virtual Pin Handlers (Appliance Switches) ───────────
// Exclusive mode: setApplianceExclusive() turns off any other
// active appliance before switching the requested one on.

BLYNK_WRITE(V5) {  // Air pump
  bool state = param.asInt();
  setApplianceExclusive(APPLIANCE_PUMP, state);
}

BLYNK_WRITE(V6) {  // LED
  bool state = param.asInt();
  setApplianceExclusive(APPLIANCE_LED, state);
}

BLYNK_WRITE(V7) {  // Seven-segment
  bool state = param.asInt();
  setApplianceExclusive(APPLIANCE_SEVENSEG, state);
}

BLYNK_WRITE(V8) {  // USB fan
  bool state = param.asInt();
  setApplianceExclusive(APPLIANCE_FAN, state);
}

// ─── Blynk Connected Callback ──────────────────────────────────
BLYNK_CONNECTED() {
  gBlynkConnected = true;
  Blynk.syncAll();
  Serial.println("[Blynk] Connected & synced.");
}

// ─── Sync Relay States Back to Blynk Switches ──────────────────
// Ensures the mobile app switches always reflect the true relay
// state, especially after exclusive-mode auto-offs.
void syncApplianceStatesToBlynk() {
  Blynk.virtualWrite(V5, isApplianceOn(APPLIANCE_PUMP)     ? 1 : 0);
  Blynk.virtualWrite(V6, isApplianceOn(APPLIANCE_LED)      ? 1 : 0);
  Blynk.virtualWrite(V7, isApplianceOn(APPLIANCE_SEVENSEG) ? 1 : 0);
  Blynk.virtualWrite(V8, isApplianceOn(APPLIANCE_FAN)      ? 1 : 0);
}

// ─── Sensor Acquisition & Publish ──────────────────────────────
void readAndPublish() {
  // --- Voltage (B25 sensor) ---
  gVoltage = measureVoltage();

  // --- Current (ACS712, zero-calibrated) ---
  gCurrent = measureCurrent();

  // --- Power ---
  gPower = calcPower(gVoltage, gCurrent);

  // --- DHT22 ---
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    gTemperature = t;
    gHumidity    = h;
  }

  // --- OLED Update ---
  updateOLED(oled, getActiveApplianceName(),
             gVoltage, gCurrent, gPower,
             gTemperature, gHumidity, gBlynkConnected);

  // --- Blynk Publish (sensor readings) ---
  Blynk.virtualWrite(V0, gVoltage);
  Blynk.virtualWrite(V1, gCurrent);
  Blynk.virtualWrite(V2, gPower);
  Blynk.virtualWrite(V3, gTemperature);
  Blynk.virtualWrite(V4, gHumidity);

  // --- Keep Blynk switches synchronized with actual relay states ---
  syncApplianceStatesToBlynk();

  // --- Serial Debug ---
  Serial.printf("[DATA] Load=%s  V=%.2fV  I=%.3fA  P=%.2fW  T=%.1fC  H=%.1f%%\n",
                getActiveApplianceName(), gVoltage, gCurrent, gPower,
                gTemperature, gHumidity);
}

// ─── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Smart Appliance Monitor - Booting ===");

  // 1. Configure relay pins, all OFF by default
  initRelays();

  // 2. I2C bus for OLED
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // 3. OLED init
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("[ERROR] OLED not found!");
  } else {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.println("Smart Monitor v2.0");
    oled.println("Calibrating...");
    oled.display();
    Serial.println("[OK] OLED initialised.");
  }

  // 4. DHT22 init
  dht.begin();
  Serial.println("[OK] DHT22 initialised.");

  // 5. ACS712 zero-current calibration - appliances must be OFF here
  //    (guaranteed, since initRelays() already switched everything off)
  calibrateACS712Zero();

  // 6. Wi-Fi + Blynk connect
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Connecting WiFi...");
  oled.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("WiFi + Blynk OK!");
  oled.display();
  delay(1000);

  // 7. Timer: poll sensors & publish every 2000 ms
  timer.setInterval(SAMPLE_INTERVAL_MS, readAndPublish);

  Serial.println("[OK] Setup complete. Entering main loop.");
}

// ─── Loop ──────────────────────────────────────────────────────
void loop() {
  gBlynkConnected = Blynk.connected();
  Blynk.run();
  timer.run();
}
