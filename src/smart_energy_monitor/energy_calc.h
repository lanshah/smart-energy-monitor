/*
 * energy_calc.h - Voltage, Current & Power Calculations
 * Smart Energy Monitoring and Appliance Control System | ACE6263
 *
 * Voltage:
 *   The B25 sensor is a resistive voltage divider (0-25V range). The ESP32
 *   ADC reads the reduced analog signal on GPIO35. The actual input voltage
 *   is reconstructed by multiplying the measured sensor output voltage by
 *   the divider ratio and an optional calibration factor.
 *
 * Current:
 *   The ACS712 outputs an analog voltage centred on a "zero-current" level.
 *   Because this zero point drifts between boards and supply conditions,
 *   the program measures it once at startup (with all appliances off) and
 *   uses that value as the reference for every subsequent reading.
 */

#ifndef ENERGY_CALC_H
#define ENERGY_CALC_H

#include <Arduino.h>
#include "config.h"

// Stores the calibrated ACS712 zero-current voltage (set once at startup)
static float gACS712ZeroVoltage = 1.65f;   // Sensible default until calibrated

// ─────────────────────────────────────────────────────────────────────────────
// adcToVoltage()
//   Converts a raw 12-bit ESP32 ADC reading into a voltage (0 - 3.3V).
// ─────────────────────────────────────────────────────────────────────────────
inline float adcToVoltage(int adcRaw) {
  return (adcRaw / ADC_RESOLUTION) * ADC_VREF;
}

// ─────────────────────────────────────────────────────────────────────────────
// calibrateACS712Zero()
//   Must be called at startup while ALL appliances/relays are switched OFF.
//   Averages ACS712_ZERO_SAMPLE_COUNT readings to obtain a stable zero-current
//   reference voltage, which compensates for board-to-board and supply
//   variations.
// ─────────────────────────────────────────────────────────────────────────────
void calibrateACS712Zero() {
  long sum = 0;
  for (int i = 0; i < ACS712_ZERO_SAMPLE_COUNT; i++) {
    sum += analogRead(CURRENT_SENSOR_PIN);
    delay(2);
  }
  int avgRaw = sum / ACS712_ZERO_SAMPLE_COUNT;
  gACS712ZeroVoltage = adcToVoltage(avgRaw);
  Serial.printf("[CAL] ACS712 zero-current voltage = %.4f V\n", gACS712ZeroVoltage);
}

// ─────────────────────────────────────────────────────────────────────────────
// measureVoltage()
//   Reads the B25 sensor output on VOLTAGE_SENSOR_PIN and returns the
//   estimated DC input voltage.
//
//   Measured voltage = Sensor output voltage x Divider ratio x Calibration factor
// ─────────────────────────────────────────────────────────────────────────────
float measureVoltage() {
  int adcRaw = analogRead(VOLTAGE_SENSOR_PIN);
  float sensorOutputV = adcToVoltage(adcRaw);
  float measuredV = sensorOutputV * VOLTAGE_DIVIDER_RATIO * VOLTAGE_CAL_FACTOR;
  return measuredV;
}

// ─────────────────────────────────────────────────────────────────────────────
// measureCurrent()
//   Reads the ACS712 output on CURRENT_SENSOR_PIN, averages several samples
//   for stability, and returns the current in amps.
//
//   Current = |ACS712 output voltage - zero-current voltage| / sensitivity
// ─────────────────────────────────────────────────────────────────────────────
float measureCurrent() {
  long sum = 0;
  for (int i = 0; i < ACS712_READ_SAMPLE_COUNT; i++) {
    sum += analogRead(CURRENT_SENSOR_PIN);
  }
  int avgRaw = sum / ACS712_READ_SAMPLE_COUNT;
  float outputV = adcToVoltage(avgRaw);

  float current = fabs(outputV - gACS712ZeroVoltage) / ACS712_SENSITIVITY_V_PER_A;
  return current;
}

// ─────────────────────────────────────────────────────────────────────────────
// calcPower()  - Power = Voltage x Current
// ─────────────────────────────────────────────────────────────────────────────
inline float calcPower(float voltage, float current) {
  return voltage * current;
}

#endif // ENERGY_CALC_H
