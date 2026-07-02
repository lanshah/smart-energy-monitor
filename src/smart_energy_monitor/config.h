/*
 * config.h - Pin Definitions & System Constants
 * Smart Energy Monitoring and Appliance Control System Using ESP32 and Blynk
 * ACE6263 - Embedded IoT Systems
 *
 * IMPORTANT: This system is designed ONLY for low-voltage DC testing.
 * Do NOT connect any part of this circuit directly to household AC mains.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ─── Wi-Fi Credentials ────────────────────────────────────────
#define WIFI_SSID   "YOUR_WIFI_SSID"
#define WIFI_PASS   "YOUR_WIFI_PASSWORD"

// ─── I2C Bus Pins (shared by OLED) ────────────────────────────
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22

// ─── OLED Display (128x64, I2C) ──────────────────────────────
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_ADDR       0x3C
#define OLED_RESET_PIN  -1     // No dedicated reset pin

// ─── B25 0-25V DC Voltage Sensor ──────────────────────────────
#define VOLTAGE_SENSOR_PIN   35    // ADC1_CH7 (input-only pin)
#define ADC_VREF             3.3f
#define ADC_RESOLUTION        4095.0f   // 12-bit ADC (0-4095)
#define VOLTAGE_DIVIDER_RATIO 5.0f      // B25 nominal divider ratio
#define VOLTAGE_CAL_FACTOR    1.0f      // Fine-tune against a multimeter

// ─── ACS712 30A Current Sensor ───────────────────────────────
#define CURRENT_SENSOR_PIN     34   // ADC1_CH6 (input-only pin)
#define ACS712_SENSITIVITY_V_PER_A   0.066f   // 66 mV/A for 30A variant
#define ACS712_ZERO_SAMPLE_COUNT     200      // Samples for zero-current calibration
#define ACS712_READ_SAMPLE_COUNT     100      // Samples per current reading

// ─── DHT22 ────────────────────────────────────────────────────
#define DHT22_PIN   32

// ─── Four-Channel Relay Module (Active-LOW) ──────────────────
#define RELAY_PUMP_PIN        26   // Relay CH1 - Air pump
#define RELAY_LED_PIN          27   // Relay CH2 - LED
#define RELAY_SEVENSEG_PIN     25   // Relay CH3 - Seven-segment (1 segment)
#define RELAY_FAN_PIN           33   // Relay CH4 - USB fan

// Appliance index constants (used across relay_ctrl.h / main sketch)
#define APPLIANCE_NONE       -1
#define APPLIANCE_PUMP        0
#define APPLIANCE_LED         1
#define APPLIANCE_SEVENSEG    2
#define APPLIANCE_FAN         3

// ─── Application Constants ───────────────────────────────────
#define SAMPLE_INTERVAL_MS   2000    // Sensor poll & Blynk publish period (ms)

// ─── Blynk Virtual Pin Map ───────────────────────────────────
// V0 → Voltage (V)             [numeric, ESP32 -> Blynk]
// V1 → Current (A)             [numeric, ESP32 -> Blynk]
// V2 → Power (W)               [numeric, ESP32 -> Blynk]
// V3 → Temperature (°C)        [numeric, ESP32 -> Blynk]
// V4 → Humidity (%)            [numeric, ESP32 -> Blynk]
// V5 → Air pump control        [integer 0/1, Blynk <-> ESP32]
// V6 → LED control              [integer 0/1, Blynk <-> ESP32]
// V7 → Seven-segment control    [integer 0/1, Blynk <-> ESP32]
// V8 → USB fan control          [integer 0/1, Blynk <-> ESP32]

#endif // CONFIG_H
