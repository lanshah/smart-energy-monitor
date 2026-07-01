# Bill of Materials (BOM)
## Smart Energy Monitoring and Appliance Control System Using ESP32 and Blynk

| # | Component | Qty | Unit Price (MYR) | Total (MYR) | Justification |
|---|-----------|-----|-----------------|-------------|---------------|
| 1 | ESP32 Dev Board | 1 | 25.00 | 25.00 | Main MCU with built-in Wi-Fi for sensor reading, relay control, OLED updates, and Blynk communication |
| 2 | B25 0–25V DC Voltage Sensor | 1 | 6.00 | 6.00 | Voltage-divider based sensor for measuring DC voltage supplied to the appliances |
| 3 | ACS712 30A Current Sensor Module | 1 | 8.50 | 8.50 | Hall-effect current sensing of the active appliance; single sensor placed in series with the common supply |
| 4 | DHT22 Temperature & Humidity Sensor | 1 | 9.00 | 9.00 | Monitors ambient temperature and humidity alongside electrical readings |
| 5 | 128×64 I2C OLED Display | 1 | 13.00 | 13.00 | Local display of active appliance, voltage, current, power, temperature, humidity and Blynk status |
| 6 | 4-Channel Relay Module | 1 | 7.50 | 7.50 | Switches the four low-voltage appliances (air pump, LED, seven-segment, USB fan) |
| 7 | DC Air Pump (3.7–6V) | 1 | 10.00 | 10.00 | Test appliance 1 |
| 8 | LED (with resistor) | 1 | 1.00 | 1.00 | Test appliance 2 |
| 9 | Dual-Digit Seven-Segment Display | 1 | 3.00 | 3.00 | Test appliance 3 (one segment used) |
| 10 | 5V USB Fan | 1 | 8.00 | 8.00 | Test appliance 4 |
| 11 | USB Breakout Board | 1 | 4.00 | 4.00 | Provides accessible VBUS/GND terminals to power the USB fan through the relay |
| 12 | Breadboard + Jumper Wire Set | 1 | 12.00 | 12.00 | Prototyping and circuit connections without soldering |
| 13 | 5V Regulated DC Power Supply | 1 | 10.00 | 10.00 | External supply for appliances, relay module and 5V sensors |
| 14 | USB-A to Micro-USB Cable | 1 | 5.00 | 5.00 | ESP32 programming and power from laptop |
| 15 | Miscellaneous (resistors, wires, connectors) | 1 lot | 8.00 | 8.00 | Current-limiting resistors for LED/segment, pull-up resistor for DHT22, connecting wires |
| | | | **TOTAL** | **112.00** | |

All components are commercially available from local electronics suppliers and priced at standard retail rates as of 2026. This is a **low-voltage DC prototype only** - none of the components or wiring are rated or intended for household AC mains use.
