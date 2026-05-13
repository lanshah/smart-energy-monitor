/*
 * energy_calc.h — RMS Voltage, Current & Power Calculations
 * Smart Energy Monitoring System | ACE6263
 *
 * ZMPT101B B25 Note:
 *   The B25 module features an integrated 25:1 toroidal transformer with a
 *   pre-fitted burden resistor. The output is a scaled, isolated replica of
 *   the mains waveform centred at Vcc/2. We collect 500 raw ADC samples per
 *   call (covering several AC cycles at 50 Hz), compute the zero-corrected
 *   RMS, then multiply by the empirically determined calibration factor to
 *   recover true RMS mains voltage.
 */

#ifndef ENERGY_CALC_H
#define ENERGY_CALC_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include "config.h"

// ─────────────────────────────────────────────────────────────────────────────
// measureVoltageRMS()
//   Reads ZMPT101B B25 output via ADS1115 and returns true RMS mains voltage.
//
//   Algorithm:
//     1. Collect N samples from ADS1115 channel `ch`.
//     2. Compute DC offset (mean of samples) for zero-crossing correction.
//     3. Subtract offset from each sample, square, sum, and take sqrt → raw RMS.
//     4. Convert raw ADC RMS to millivolts using ADS full-scale range.
//     5. Apply ZMPT101B_CALIBRATION_FACTOR to get true AC RMS in volts.
//
//   @param ads      ADS1115 instance (must be initialised with GAIN_ONE)
//   @param ch       ADS1115 channel (0 for ZMPT101B)
//   @param calFactor Calibration multiplier for the B25 module
//   @return         True RMS mains voltage in volts
// ─────────────────────────────────────────────────────────────────────────────
float measureVoltageRMS(Adafruit_ADS1115 &ads, uint8_t ch, float calFactor) {
  long   sum       = 0;
  long   sqSum     = 0;
  int16_t samples[ZMPT101B_SAMPLE_COUNT];
  const int N = ZMPT101B_SAMPLE_COUNT;

  // Pass 1: collect & accumulate for mean
  for (int i = 0; i < N; i++) {
    samples[i] = ads.readADC_SingleEnded(ch);
    sum += samples[i];
  }
  float mean = (float)sum / N;

  // Pass 2: zero-correct and compute RMS
  float sumSq = 0.0f;
  for (int i = 0; i < N; i++) {
    float v = (float)samples[i] - mean;
    sumSq += v * v;
  }
  float rmsADC = sqrt(sumSq / N);

  // Convert ADC counts → mV → V, apply calibration
  // ADS1115 with GAIN_ONE: 1 bit = 0.125 mV
  float rmsMV = rmsADC * 0.125f;
  float rmsV  = rmsMV * calFactor;

  return rmsV;
}

// ─────────────────────────────────────────────────────────────────────────────
// measureCurrentRMS()
//   Reads ACS712 30A output via ADS1115 and returns true RMS current in amps.
//
//   Algorithm:
//     1. Collect N samples from ADS1115 channel `ch`.
//     2. Compute quiescent offset (mean) — replaces fixed midpoint to account
//        for supply voltage variations.
//     3. Subtract offset, convert to mV, divide by sensitivity to get amps.
//     4. Compute RMS of corrected current waveform.
//
//   @param ads          ADS1115 instance
//   @param ch           ADS1115 channel (1 for ACS712)
//   @param sensitivity  mV per Amp (66.0 for 30A variant)
//   @param midpointMV   Nominal quiescent output in mV (unused — auto-computed)
//   @param vrefMV       ADS full-scale in mV (4096 for GAIN_ONE)
//   @return             True RMS current in amps
// ─────────────────────────────────────────────────────────────────────────────
float measureCurrentRMS(Adafruit_ADS1115 &ads, uint8_t ch,
                         float sensitivity, float midpointMV, float vrefMV) {
  int16_t samples[ACS712_SAMPLE_COUNT];
  long    sum = 0;
  const int N = ACS712_SAMPLE_COUNT;

  // Collect samples
  for (int i = 0; i < N; i++) {
    samples[i] = ads.readADC_SingleEnded(ch);
    sum += samples[i];
  }
  float meanADC = (float)sum / N;

  // Compute RMS after zero-correction
  float sumSq = 0.0f;
  for (int i = 0; i < N; i++) {
    // ADC count → mV (0.125 mV/bit with GAIN_ONE)
    float deltaADC = (float)samples[i] - meanADC;
    float deltaMV  = deltaADC * 0.125f;
    float amps     = deltaMV / sensitivity;
    sumSq += amps * amps;
  }
  float rmsA = sqrt(sumSq / N);

  // Filter noise floor (< 20 mA treated as zero)
  if (rmsA < 0.02f) rmsA = 0.0f;

  return rmsA;
}

// ─────────────────────────────────────────────────────────────────────────────
// calcApparentPower()  — V(rms) × I(rms)
// calcRealPower()      — apparentPower × powerFactor
// calcEnergyKWh()      — incremental energy in kWh for elapsed seconds
// ─────────────────────────────────────────────────────────────────────────────
inline float calcApparentPower(float vRMS, float iRMS) {
  return vRMS * iRMS;
}

inline float calcRealPower(float apparentPower, float powerFactor) {
  return apparentPower * powerFactor;
}

inline float calcEnergyKWh(float powerW, float elapsedSeconds) {
  return (powerW / 1000.0f) * (elapsedSeconds / 3600.0f);
}

#endif // ENERGY_CALC_H
