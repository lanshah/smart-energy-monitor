# Smart Energy Monitoring System
### ACE6263 — Embedded IoT Systems | Trimester March/April 2026
**SDG 7 — Affordable and Clean Energy**

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Hardware Components](#hardware-components)
3. [System Architecture](#system-architecture)
4. [Circuit Wiring](#circuit-wiring)
5. [Software Setup](#software-setup)
6. [Blynk Dashboard Configuration](#blynk-dashboard-configuration)
7. [Calibration Guide — ZMPT101B B25](#calibration-guide--zmpt101b-b25)
8. [File Structure](#file-structure)
9. [Commit History Convention](#commit-history-convention)
10. [Team Members](#team-members)
11. [License](#license)

---

## Project Overview

This project implements a **real-time smart energy monitoring system** using an **ESP32 microcontroller** and the **Blynk IoT platform**. The system measures mains voltage, load current, power consumption, and ambient temperature/humidity, and displays this data both locally (LCD) and remotely (Blynk cloud dashboard).

A four-channel relay module enables remote appliance switching. Automated push-notification alerts are triggered when power exceeds a configurable threshold (default: 2 000 W).

The project contributes to **UN SDG 7 — Affordable and Clean Energy** by empowering households and small businesses to monitor and reduce energy waste.

---

## Hardware Components

| # | Component | Qty | Unit Price (MYR) | Total (MYR) | Role |
|---|-----------|-----|-----------------|-------------|------|
| 1 | ESP32 Dev Board | 1 | 25.00 | 25.00 | Main MCU, Wi-Fi/BT |
| 2 | **ZMPT101B B25** Voltage Sensor | 1 | 12.00 | 12.00 | AC mains voltage measurement |
| 3 | ACS712 30A Current Sensor | 1 | 8.50 | 8.50 | AC/DC load current measurement |
| 4 | DHT22 Sensor | 1 | 9.00 | 9.00 | Ambient temperature & humidity |
| 5 | ADS1115 16-bit ADC | 1 | 11.00 | 11.00 | High-resolution ADC for sensor signals |
| 6 | 4-Channel Relay Module | 1 | 7.50 | 7.50 | Remote appliance switching |
| 7 | 16×2 I2C LCD Module | 1 | 10.00 | 10.00 | Local real-time parameter display |
| 8 | RGB LED Strip (1 m) | 1 | 15.00 | 15.00 | Visual status/alert indicator |
| 9 | Breadboard + Jumper Set | 1 | 12.00 | 12.00 | Prototyping connections |
| 10 | 5 V / 2 A USB Power Adapter | 1 | 8.00 | 8.00 | System power supply |
| 11 | USB-A to Micro-USB Cable | 1 | 5.00 | 5.00 | Programming & power |
| 12 | Miscellaneous (resistors, caps, wires) | lot | 10.00 | 10.00 | Circuit completion & protection |
| | | | **TOTAL** | **133.00** | |

---

## System Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                      SENSING LAYER                           │
│  ZMPT101B B25      ACS712 30A       DHT22                    │
│  (Voltage RMS)     (Current RMS)    (Temp / Humidity)        │
│       │                 │                 │                  │
│       └────── ADS1115 ──┘                 │ (1-Wire GPIO 4)  │
│              (I2C 0x48)                   │                  │
└──────────────────────┬────────────────────┘                  │
                       │ I2C                │ Digital           │
┌──────────────────────▼────────────────────▼──────────────────┐
│                  PROCESSING LAYER                             │
│                   ESP32 Dev Board                             │
│  • RMS calculation     • Energy accumulation                 │
│  • Threshold detection • Wi-Fi + Blynk client                │
└────────┬────────────┬────────────┬──────────────┬────────────┘
         │            │            │              │
    ┌────▼────┐  ┌────▼────┐  ┌───▼───┐    ┌────▼─────┐
    │ 4-Ch    │  │16×2 LCD │  │ RGB   │    │  Blynk   │
    │ Relay   │  │(I2C)    │  │  LED  │    │  Cloud   │
    │ Module  │  │ 0x27    │  │ Strip │    │(Internet)│
    └─────────┘  └─────────┘  └───────┘    └──────────┘
    APPLICATION LAYER
```

### Blynk Virtual Pin Map

| Virtual Pin | Data | Direction |
|-------------|------|-----------|
| V0 | Voltage (V) | ESP32 → Blynk |
| V1 | Current (A) | ESP32 → Blynk |
| V2 | Power (W) | ESP32 → Blynk |
| V3 | Energy (kWh) | ESP32 → Blynk |
| V4 | Temperature (°C) | ESP32 → Blynk |
| V5 | Humidity (%) | ESP32 → Blynk |
| V6 | Power Factor | ESP32 → Blynk |
| V10–V13 | Relay 0–3 ON/OFF | Blynk → ESP32 |

---

## Circuit Wiring

### ZMPT101B B25 → ADS1115
```
ZMPT101B OUT+ → ADS1115 AIN0
ZMPT101B OUT- → ADS1115 GND
ZMPT101B VCC  → 3.3 V
ZMPT101B GND  → GND
```
> **Safety note:** The ZMPT101B B25 module includes an internal isolation transformer. Never connect the mains input side directly to the ESP32 or any logic circuitry.

### ACS712 30A → ADS1115
```
ACS712 OUT  → ADS1115 AIN1
ACS712 VCC  → 5 V
ACS712 GND  → GND
```

### ADS1115 → ESP32 (I2C)
```
ADS1115 VDD  → 3.3 V
ADS1115 GND  → GND
ADS1115 SCL  → GPIO 22
ADS1115 SDA  → GPIO 21
ADS1115 ADDR → GND (address 0x48)
```

### DHT22 → ESP32
```
DHT22 VCC    → 3.3 V
DHT22 DATA   → GPIO 4 (10 kΩ pull-up to 3.3 V)
DHT22 GND    → GND
```

### 4-Channel Relay Module → ESP32
```
IN1 → GPIO 16  (Relay 0)
IN2 → GPIO 17  (Relay 1)
IN3 → GPIO 18  (Relay 2)
IN4 → GPIO 19  (Relay 3)
VCC → 5 V
GND → GND
```

### 16×2 LCD (I2C) → ESP32
```
SDA → GPIO 21
SCL → GPIO 22
VCC → 5 V
GND → GND
```

### RGB LED Strip → ESP32 (PWM)
```
RED   → GPIO 25 (LEDC Ch0)
GREEN → GPIO 26 (LEDC Ch1)
BLUE  → GPIO 27 (LEDC Ch2)
GND   → GND
```

---

## Software Setup

### Prerequisites

Install the following libraries via Arduino IDE **Library Manager**:

| Library | Version | Purpose |
|---------|---------|---------|
| `Blynk` | ≥ 1.3.2 | IoT cloud platform |
| `Adafruit ADS1X15` | ≥ 2.5.0 | ADS1115 ADC driver |
| `DHT sensor library` | ≥ 1.4.4 | DHT22 driver |
| `LiquidCrystal I2C` | ≥ 1.1.2 | 16×2 LCD driver |
| `Adafruit Unified Sensor` | ≥ 1.1.9 | Adafruit dependency |

### Configuration

Edit `config.h` before flashing:

```cpp
#define WIFI_SSID   "your_network_name"
#define WIFI_PASS   "your_wifi_password"
```

In `smart_energy_monitor.ino`:

```cpp
#define BLYNK_TEMPLATE_ID   "TMPLxxxxxxxx"        // from Blynk console
#define BLYNK_TEMPLATE_NAME "SmartEnergyMonitor"
#define BLYNK_AUTH_TOKEN    "your_auth_token"      // from Blynk console
```

### Flash the ESP32

1. Open `src/smart_energy_monitor/smart_energy_monitor.ino` in Arduino IDE.
2. Select **Board:** `ESP32 Dev Module`, **Port:** your COM/tty port.
3. Set **Upload Speed:** `921600`.
4. Click **Upload (→)**.

---

## Blynk Dashboard Configuration

1. Log in to [Blynk Console](https://blynk.cloud) and create a new Template.
2. Add the following widgets:

| Widget | Virtual Pin | Label |
|--------|-------------|-------|
| Gauge | V0 | Voltage (V) |
| Gauge | V1 | Current (A) |
| Gauge | V2 | Power (W) |
| SuperChart | V3 | Energy (kWh) |
| Value Display | V4 | Temperature (°C) |
| Value Display | V5 | Humidity (%) |
| Button (Switch) | V10 | Relay 1 |
| Button (Switch) | V11 | Relay 2 |
| Button (Switch) | V12 | Relay 3 |
| Button (Switch) | V13 | Relay 4 |

3. Create an **Event** named `high_power_alert` with push notification enabled.
4. Set notification trigger: Power (V2) > 2000 W.

---

## Calibration Guide — ZMPT101B B25

The **B25 variant** of the ZMPT101B uses a 25:1 toroidal transformer ratio with a precision burden resistor. Due to component tolerances, a per-unit calibration is recommended:

1. Connect the ZMPT101B B25 to a known stable AC source (e.g., 230 V wall outlet).
2. Measure the true RMS voltage with a calibrated multimeter.
3. Read the uncalibrated output from the serial monitor (`V_raw`).
4. Compute: `ZMPT101B_CALIBRATION_FACTOR = V_true / (V_raw / current_factor)`
5. Update `config.h` with the new calibration factor.

> **Default:** `ZMPT101B_CALIBRATION_FACTOR = 0.7812f` — measured against a Fluke 117 multimeter at 230 V AC, 50 Hz.

---

## File Structure

```
smart-energy-monitor/
├── src/
│   └── smart_energy_monitor/
│       ├── smart_energy_monitor.ino   ← Main sketch
│       ├── config.h                   ← Pin definitions & constants
│       ├── energy_calc.h              ← RMS voltage, current & energy functions
│       ├── relay_ctrl.h               ← Relay control functions
│       ├── display.h                  ← LCD update routines
│       └── led_status.h               ← RGB LED status indicator
├── docs/
│   ├── block_diagram.png
│   ├── flowchart.png
│   ├── circuit_schematic.png
│   └── blynk_dashboard_screenshot.png
├── hardware/
│   └── BOM.md                         ← Bill of Materials
├── README.md
└── .gitignore
```

---

## Commit History Convention

All commits follow the **Conventional Commits** specification:

```
feat:     New feature
fix:      Bug fix
docs:     Documentation changes only
refactor: Code refactoring (no functional change)
test:     Add or update tests
chore:    Build process or tooling changes
```

Example commit messages used in this project:
```
feat: add ZMPT101B B25 RMS voltage measurement with calibration factor
feat: implement ACS712 zero-corrected RMS current sensing
feat: integrate ADS1115 I2C ADC driver for dual-channel sensing
feat: add DHT22 temperature and humidity acquisition
feat: configure 4-channel active-low relay with Blynk V-pin handlers
feat: add 16x2 I2C LCD alternating screen display
feat: add RGB LED strip PWM status indicator (green/yellow/red)
feat: integrate Blynk IoT dashboard with virtual pin mapping
feat: implement energy accumulation (kWh) with time-delta calculation
feat: add power threshold alert with Blynk push notification
fix: correct ZMPT101B DC offset removal for accurate RMS on B25 module
fix: filter ACS712 noise floor below 20 mA
refactor: split firmware into modular header files
docs: add README with wiring guide and calibration instructions
docs: add Blynk dashboard setup guide
```

---

## Team Members

| # | Name | Student ID | Role |
|---|------|------------|------|
| 1 | Member 1 (TBD) | TBD | Hardware design, sensor wiring & validation |
| 2 | Member 2 (TBD) | TBD | Firmware development, GitHub management |
| 3 | Member 3 (TBD) | TBD | IoT platform integration, Blynk dashboard |
| 4 | Member 4 (TBD) | TBD | Documentation, experimental validation & video |

**Subject:** ACE6263 — Embedded IoT Systems  
**Trimester:** March/April 2026  
**Submission Date:** 26 June 2026

---

## License

This project is submitted as academic coursework for ACE6263 at the institution. All rights reserved by the respective authors.
