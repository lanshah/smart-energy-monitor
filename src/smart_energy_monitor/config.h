/*
 * config.h — Pin Definitions & System Constants
 * Smart Energy Monitoring System | ACE6263
 */

#ifndef CONFIG_H
#define CONFIG_H

// ─── Wi-Fi Credentials ────────────────────────────────────────
#define WIFI_SSID   "YOUR_WIFI_SSID"
#define WIFI_PASS   "YOUR_WIFI_PASSWORD"

// ─── I2C Bus Pins (ESP32 default) ────────────────────────────
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22

// ─── ADS1115 ─────────────────────────────────────────────────
#define ADS1115_ADDR        0x48
#define ADS_VOLTAGE_CH      0       // ZMPT101B B25 on AIN0
#define ADS_CURRENT_CH      1       // ACS712 on AIN1
#define ADS_VREF_MV         4096.0f // ±4.096 V with GAIN_ONE

// ─── ZMPT101B B25 Voltage Sensor ─────────────────────────────
// The B25 variant uses a 25:1 turns ratio burden resistor network.
// Calibration factor is tuned against a reference multimeter.
// Adjust ZMPT101B_CALIBRATION_FACTOR if measured V drifts from true RMS.
#define ZMPT101B_CALIBRATION_FACTOR   0.7812f   // Tuned for B25 module at 230 V AC
#define ZMPT101B_SAMPLE_COUNT         500        // Samples per RMS cycle

// ─── ACS712 30A Current Sensor ───────────────────────────────
#define ACS712_SENSITIVITY_MV_PER_A   66.0f     // 66 mV/A for 30 A variant
#define ACS712_MIDPOINT_MV            2500.0f   // 2.5 V quiescent (Vcc/2)
#define ACS712_SAMPLE_COUNT           500

// ─── DHT22 ────────────────────────────────────────────────────
#define DHT22_PIN   4

// ─── 4-Channel Relay (Active-LOW) ────────────────────────────
#define RELAY_PIN_0   16
#define RELAY_PIN_1   17
#define RELAY_PIN_2   18
#define RELAY_PIN_3   19

// ─── LCD I2C Address ──────────────────────────────────────────
#define LCD_ADDR    0x27

// ─── RGB LED Strip PWM Channels ──────────────────────────────
#define LED_RED_PIN     25
#define LED_GREEN_PIN   26
#define LED_BLUE_PIN    27
#define LED_PWM_FREQ    5000
#define LED_PWM_RES     8           // 8-bit: 0-255

// ─── Application Constants ───────────────────────────────────
#define POWER_ALERT_THRESHOLD_W   2000.0f   // Alert if load > 2 kW
#define SAMPLE_INTERVAL_MS        2000      // Sensor poll period (ms)

// ─── Blynk Virtual Pin Map ───────────────────────────────────
// V0  → Voltage (V)
// V1  → Current (A)
// V2  → Power (W)
// V3  → Energy (kWh)
// V4  → Temperature (°C)
// V5  → Humidity (%)
// V6  → Power Factor
// V10 → Relay 0 control
// V11 → Relay 1 control
// V12 → Relay 2 control
// V13 → Relay 3 control

#endif // CONFIG_H
