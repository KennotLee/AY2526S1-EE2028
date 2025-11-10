/*
 * MotionLite.h
 *
 *  Created on: Nov 1, 2025
 *      Author: kenne
 */

// MotionLite.h
// Minimal per-tick motion check (no start/stop state machine)
// Compare accel delta vs previous sample and gyro magnitude.
// Ken @ EE2028

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Tune defaults here (units: g and deg/s)
#ifndef MOTIONLITE_DEFAULT_ACC_DELTA_TH_G
#define MOTIONLITE_DEFAULT_ACC_DELTA_TH_G  (2.67f)  // ~0.06–0.12 g typical
#endif
#ifndef MOTIONLITE_DEFAULT_GYRO_MAG_TH_DPS
#define MOTIONLITE_DEFAULT_GYRO_MAG_TH_DPS (167.0f)  // ~15–25 dps typical
#endif

typedef struct {
    // thresholds
    float acc_delta_th_g;    // threshold on |a_t - a_{t-1}| (in g)
    float gyro_mag_th_dps;   // threshold on ||gyro|| (in deg/s)
    // state
    float prev_ax, prev_ay, prev_az; // previous accel sample (in g)
    bool  have_prev;                 // false until first sample seen
} MotionLite;

/** Initialize with sensible defaults. */
void motionlite_init(MotionLite* m);

/**
 * Optionally override thresholds at runtime (pass <0 to keep current).
 * Returns current thresholds after update.
 */
void motionlite_set_thresholds(MotionLite* m, float acc_delta_th_g, float gyro_mag_th_dps);

/**
 * Call once per sample (your cadence ~0.5 s).
 * ax, ay, az in g; gx, gy, gz in deg/s.
 * Returns true if THIS tick indicates motion (no hysteresis/history).
 */
bool motionlite_tick(MotionLite* m,
                     float ax, float ay, float az,
                     float gx, float gy, float gz);

/** Utility: get last accel delta and gyro magnitude from the most recent tick. */
typedef struct {
    float acc_delta_g;
    float gyro_mag_dps;
} MotionLiteLastMetrics;

void motionlite_get_last_metrics(const MotionLite* m,
                                 MotionLiteLastMetrics* out_metrics);

#ifdef __cplusplus
} // extern "C"
#endif
