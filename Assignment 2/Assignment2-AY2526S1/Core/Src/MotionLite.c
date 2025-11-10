/*
 * MotionLite.c
 *
 *  Created on: Nov 1, 2025
 *      Author: kenne
 */


// MotionLite.c
#include "MotionLite.h"
#include <math.h>
#include "stdio.h"
#include "string.h"

static inline float v3mag(float x, float y, float z) {
    return sqrtf(x*x + y*y + z*z);
}

// Keep last computed metrics for optional inspection (file-scope static)
static float s_last_acc_delta_g = 0.0f;
static float s_last_gyro_mag_dps = 0.0f;

void motionlite_init(MotionLite* m) {
    if (!m) return;
    m->acc_delta_th_g   = MOTIONLITE_DEFAULT_ACC_DELTA_TH_G;
    m->gyro_mag_th_dps  = MOTIONLITE_DEFAULT_GYRO_MAG_TH_DPS;
    m->prev_ax = m->prev_ay = m->prev_az = 0.0f;
    m->have_prev = false;
    s_last_acc_delta_g = 0.0f;
    s_last_gyro_mag_dps = 0.0f;
    printf("%.2f", m -> acc_delta_th_g);
    printf("%.2f", m -> gyro_mag_th_dps);
}

void motionlite_set_thresholds(MotionLite* m, float acc_delta_th_g, float gyro_mag_th_dps) {
    if (!m) return;
    if (acc_delta_th_g  >= 0.0f) m->acc_delta_th_g  = acc_delta_th_g;
    if (gyro_mag_th_dps >= 0.0f) m->gyro_mag_th_dps = gyro_mag_th_dps;


}

bool motionlite_tick(MotionLite* m,
                     float ax, float ay, float az,
                     float gx, float gy, float gz)
{
    if (!m) return false;

    float acc_delta = 0.0f;
    if (m->have_prev) {
        acc_delta = v3mag(ax - m->prev_ax,
                          ay - m->prev_ay,
                          az - m->prev_az);
    } else {
        m->have_prev = true; // first sample has no meaningful delta
    }

    float gyro_mag = v3mag(gx, gy, gz);

    // Save for optional inspection
    s_last_acc_delta_g   = acc_delta;
    s_last_gyro_mag_dps  = gyro_mag;

    // Update previous accel
    m->prev_ax = ax; m->prev_ay = ay; m->prev_az = az;

    // Decide motion for THIS tick only
    return (acc_delta >= m->acc_delta_th_g) || (gyro_mag >= m->gyro_mag_th_dps);
}

void motionlite_get_last_metrics(const MotionLite* m,
                                 MotionLiteLastMetrics* out_metrics)
{
    (void)m; // currently stored globally; keep signature for future per-instance storage
    if (!out_metrics) return;
    out_metrics->acc_delta_g    = s_last_acc_delta_g;
    out_metrics->gyro_mag_dps   = s_last_gyro_mag_dps;
}
