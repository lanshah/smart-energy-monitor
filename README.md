# Smart Energy Monitoring and Appliance Control System Using ESP32 and Blynk
### ACE6263 - Embedded IoT Systems | Trimester March/April 2026

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Objectives](#objectives)
3. [Hardware Components](#hardware-components)
4. [ESP32 Pin Assignment](#esp32-pin-assignment)
5. [Circuit Wiring](#circuit-wiring)
6. [Power Distribution](#power-distribution)
7. [Software Setup](#software-setup)
8. [Blynk Dashboard Configuration](#blynk-dashboard-configuration)
9. [Exclusive Appliance-Control Mode](#exclusive-appliance-control-mode)
10. [Data Processing / Calibration](#data-processing--calibration)
11. [File Structure](#file-structure)
12. [Limitations](#limitations)
13. [Team Members](#team-members)

---

## Project Overview

This project is an Internet of Things system built around an **ESP32** that
monitors electrical and environmental conditions while allowing a user to
remotely control four low-voltage appliances using the **Blynk** mobile
application.

**⚠️ Safety note:** This system is designed only for **low-voltage DC
testing** and must **never** be connected directly to household AC mains
electricity.

The system measures DC voltage, DC current, electrical power, temperature,
and humidity, and controls:

- A 3.7–6 V DC air pump
- An LED
- One selected segment of a dual-digit seven-segment display
- A 5 V USB fan (via USB breakout board)

Only **one appliance can be active at a time** (exclusive mode), because a
single ACS712 current sensor is used to measure whichever appliance is
currently powered.

---

## Objectives

1. Measure the supply voltage of the connected load.
2. Measure the current consumed by the active appliance.
3. Calculate power using `Power = Voltage × Current`.
4. Measure surrounding temperature and humidity.
5. Display readings locally on an OLED display.
6. Send readings to the Blynk IoT platform via Wi-Fi.
7. Allow users to remotely switch appliances on/off via the Blynk app.
8. Show the currently active appliance.
9. Allow only one appliance to operate at a time.

---

## Hardware Components

| Component | Function |
|-----------|----------|
| ESP32 Dev Board | Main controller - reads sensors, calculates power, controls relays, updates OLED, connects to Wi-Fi/Blynk |
| B25 0–25V DC Voltage Sensor | Measures DC voltage supplied to appliances (voltage-divider based) |
| ACS712 30A Current Sensor | Measures current consumed by the active appliance |
| DHT22 Sensor | Measures ambient temperature and humidity |
| 128×64 I2C OLED Display | Local display of all readings and active appliance |
| 4-Channel Relay Module | Switches the four appliances (active-LOW) |
| DC Air Pump (3.7–6V) | Appliance 1 |
| LED | Appliance 2 |
| Dual-digit Seven-Segment Display (1 segment used) | Appliance 3 |
| 5V USB Fan + USB Breakout Board | Appliance 4 |

See [`hardware/BOM.md`](hardware/BOM.md) for full pricing and justifications.

---

## ESP32 Pin Assignment

| GPIO | Function |
|------|----------|
| GPIO35 | B25 voltage sensor signal |
| GPIO34 | ACS712 current sensor output |
| GPIO32 | DHT22 data |
| GPIO21 | OLED SDA |
| GPIO22 | OLED SCL |
| GPIO26 | Relay Channel 1 - Air pump |
| GPIO27 | Relay Channel 2 - LED |
| GPIO25 | Relay Channel 3 - Seven-segment |
| GPIO33 | Relay Channel 4 - USB fan |

---

## Circuit Wiring

### B25 Voltage Sensor → ESP32
```
B25 Signal Out → GPIO35
B25 VCC        → 3.3V (or 5V, per module spec)
B25 GND        → Common GND
```
The B25 acts as a voltage divider. The ESP32 reads the reduced analog
signal and reconstructs the actual input voltage using the divider ratio
(≈5.0, calibrate against a multimeter).

### ACS712 30A Current Sensor → ESP32
```
ACS712 OUT → GPIO34
ACS712 VCC → 5V
ACS712 GND → Common GND
```
The positive supply passes **through** the ACS712 current terminals before
reaching the relay COM terminals, so the current of the selected appliance
is measured by this single sensor.

### DHT22 → ESP32
```
DHT22 VCC  → 3.3V
DHT22 DATA → GPIO32 (10kΩ pull-up to 3.3V)
DHT22 GND  → Common GND
```

### OLED (128×64, I2C) → ESP32
```
SDA → GPIO21
SCL → GPIO22
VCC → 3.3V
GND → Common GND
```

### 4-Channel Relay Module → ESP32
```
IN1 → GPIO26  (Air pump)
IN2 → GPIO27  (LED)
IN3 → GPIO25  (Seven-segment)
IN4 → GPIO33  (USB fan)
VCC → 5V
GND → Common GND
```
Relay module is **active-LOW**: LOW = relay ON, HIGH = relay OFF. Loads are
wired through COM and NO contacts, so appliances stay off until their
channel is activated.

### Appliance Wiring

**Air pump (Relay CH1):**
```
External 5V+ (after ACS712) → Relay CH1 COM
Relay CH1 NO                → Air pump (+)
Air pump (–)                → External supply GND
```

**LED (Relay CH2):**
```
External 5V+ (after ACS712) → Relay CH2 COM
Relay CH2 NO → current-limiting resistor → LED (+)
LED (–)      → External supply GND
```

**Seven-segment, one segment (Relay CH3):**
```
External 5V+ (after ACS712) → Relay CH3 COM
Relay CH3 NO → 220-330Ω resistor → chosen segment pin
Display pin 8 (common)          → External supply GND
```
Only one segment is used; the display does not show complete numbers.

**USB fan (Relay CH4):**
```
External 5V+ (after ACS712)      → Relay CH4 COM
Relay CH4 NO                      → USB breakout VBUS / 5V pin
USB breakout GND                  → External supply GND
(USB D+ / D− pins are not connected)
```

---

## Power Distribution

- The **ESP32** is powered via laptop USB during testing.
- The **appliances, relay module, and 5V sensors** are powered from an
  **external regulated 5V DC supply**.
- **All grounds are commoned**: ESP32, external supply, relay module,
  ACS712, B25, DHT22, OLED, air pump, LED, seven-segment, USB breakout.
- Main positive path:
  ```
  External 5V (+) → ACS712 IN → ACS712 OUT → Relay COM terminals (all 4 channels)
  ```
  This lets the single ACS712 measure whichever appliance is currently
  active.

---

## Software Setup

### Required Libraries (Arduino IDE Library Manager)

| Library | Purpose |
|---------|---------|
| `Blynk` | IoT cloud platform |
| `DHT sensor library` | DHT22 driver |
| `Adafruit GFX Library` | Graphics primitives for OLED |
| `Adafruit SSD1306` | 128×64 OLED driver |

### Configuration

Edit `config.h`:
```cpp
#define WIFI_SSID   "your_network_name"
#define WIFI_PASS   "your_wifi_password"
```

Edit the top of `smart_energy_monitor.ino`:
```cpp
#define BLYNK_TEMPLATE_ID   "TMPLxxxxxxxx"     // from Blynk console
#define BLYNK_TEMPLATE_NAME "SmartApplianceMonitor"
#define BLYNK_AUTH_TOKEN    "your_auth_token"   // from Blynk console
```

### Flash the ESP32

1. Open `src/smart_energy_monitor/smart_energy_monitor.ino` in Arduino IDE.
2. Board: `ESP32 Dev Module`. Select the correct COM/tty port.
3. Upload speed: `921600`.
4. Click **Upload**.

---

## Blynk Dashboard Configuration

Create a Blynk template with the following datastreams and widgets:

| Virtual Pin | Datastream Type | Widget | Label |
|-------------|-----------------|--------|-------|
| V0 | Numeric | Gauge / Value display | Voltage (V) |
| V1 | Numeric | Gauge / Value display | Current (A) |
| V2 | Numeric | Gauge / Value display | Power (W) |
| V3 | Numeric | Value display | Temperature (°C) |
| V4 | Numeric | Value display | Humidity (%) |
| V5 | Integer (0/1) | Switch | Air pump |
| V6 | Integer (0/1) | Switch | LED |
| V7 | Integer (0/1) | Switch | Seven-segment |
| V8 | Integer (0/1) | Switch | USB fan |

Because the firmware writes back to V5–V8 on every publish cycle, the mobile
switches automatically stay synchronized with the actual relay states -
including when exclusive mode auto-switches another appliance off.

---

## Exclusive Appliance-Control Mode

Only **one appliance** may be active at a time, because a single ACS712
sensor is used. When a user turns on a new appliance:

1. The firmware checks if a different appliance is currently active.
2. If so, that appliance's relay is switched off first.
3. The newly requested appliance's relay is then switched on.
4. Updated switch states are pushed back to Blynk so the app UI stays in
   sync.

Example: if the air pump is ON and the user switches on the USB fan, the
pump is turned off automatically before the fan is activated.

---

## Data Processing / Calibration

### Voltage
```
Measured voltage = Sensor output voltage × Divider ratio × Calibration factor
```
Sensor output voltage is derived from the ESP32's 12-bit ADC reading and the
3.3 V reference. The divider ratio defaults to ~5.0 and should be calibrated
against a multimeter (`VOLTAGE_CAL_FACTOR` in `config.h`).

### Current
```
Current = |ACS712 output voltage − zero-current voltage| / sensitivity
```
The zero-current voltage is measured automatically at startup while all
appliances are off (`calibrateACS712Zero()` in `energy_calc.h`). Sensitivity
for the 30A ACS712 variant is ≈0.066 V/A.

### Power
```
Power = Voltage × Current
```
Result is displayed in watts.

---

## File Structure

```
smart-energy-monitor/
├── src/
│   └── smart_energy_monitor/
│       ├── smart_energy_monitor.ino   ← Main sketch
│       ├── config.h                   ← Pin definitions & constants
│       ├── energy_calc.h              ← Voltage, current & power calculations
│       ├── relay_ctrl.h               ← Exclusive-mode relay control
│       └── display.h                  ← OLED display routine
├── docs/
│   ├── GITHUB_WORKFLOW.md
│   ├── block_diagram.png
│   ├── circuit_schematic.png
│   └── blynk_dashboard_screenshot.png
├── hardware/
│   └── BOM.md
├── README.md
└── .gitignore
```

---

## Limitations

- The ACS712 30A sensor has low sensitivity for very small loads (e.g. a
  single LED or seven-segment segment may be hard to measure accurately).
- Only one appliance's current can be measured at a time (single sensor).
- The system depends on Wi-Fi and Blynk Cloud for remote operation.
- Designed only for low-voltage DC appliances - not for AC mains.
- Readings require calibration against a multimeter/current meter.
- The seven-segment display uses only one segment; it does not show full
  numbers.
- Electrical noise from motors (pump, fan) may affect sensor stability.
- If a load bypasses the ACS712, its current will not be measured.

---

## Team Members

| # | Name | Student ID | Email | Major | Role Title | Contribution |
|---|------|------------|-------|-------|------------|--------------|
| 1 | Muhammad Azlan Shah Bin Azman | 1211112302 | 1211112302@student.mmu.edu.my | TE | **Hardware & Integration Lead** | B25 & ACS712 wiring and calibration; hardware-software integration |
| 2 | Mohd Aiman Najwan Bin Mohd Asri | 1211112324 | 1211112324@student.mmu.edu.my | TE | **Firmware Lead / GitHub Manager** | Core firmware development; repository and branch management |
| 3 | Luqman Bin Mohamad Ali | 1221305890 | 1221305890@student.mmu.edu.my | TE | **IoT Integration Lead** | Blynk dashboard and exclusive-mode switch synchronization |
| 4 | Amirul Fareez Bin Mohammad Faizal | 1211112318 | 1211112318@student.mmu.edu.my | TE | **Project Coordinator / Documentation Lead** | Task tracking and scheduling; documentation, testing, and demonstration video |

**Subject:** ACE6263 - Embedded IoT Systems
**Trimester:** March/April 2026
