#ifndef _OSCILLIATOR_H
#define _OSCILLIATOR_H

#include <pico/stdlib.h>
#include <hardware/interp.h>
#include "wavetable.h"

interp_config oscilliator_global_lane0_config;
interp_config oscilliator_global_lane1_config;

static inline void oscilliator_global_init() {
  oscilliator_global_lane0_config = interp_default_config();
  interp_config_set_blend(&oscilliator_global_lane0_config, true);

  oscilliator_global_lane1_config = interp_default_config();
  interp_config_set_signed(&oscilliator_global_lane1_config, true);
}

/*
 * Oscilliator
 */
typedef struct {
  wavetable_t *wavetable;
  uint32_t sample_pos;
  uint32_t increment;
} oscilliator;

static inline oscilliator oscilliator_get_default() {
  oscilliator osc;
  osc.wavetable = NULL;
  osc.sample_pos = 0;
  osc.increment = 0;
  return osc;
}

static inline oscilliator oscilliator_new(wavetable_t *wavetable,
                                          uint32_t increment) {
  oscilliator osc;
  osc.wavetable = wavetable;
  osc.sample_pos = 0;
  osc.increment = increment;
  return osc;
}

static inline void oscilliator_reset(oscilliator *osc) { osc->sample_pos = 0; }

static inline int16_t oscilliator_update(oscilliator *osc) {
  uint32_t npos = osc->sample_pos >> 16;
  uint32_t next_npos = npos + 1;
  if (next_npos >= osc->wavetable->length) {
    next_npos = 0;
  }

  uint16_t weight = osc->sample_pos;
/*
  int64_t sample_a = (int64_t)osc->wavetable->data[npos] << 16;
  int64_t sample_b = (int64_t)osc->wavetable->data[next_npos] << 16;
  int64_t lerp_sample =
      ((int64_t)sample_b * weight) + ((int64_t)sample_a * (65535UL - weight));*/
  // int16_t sample = osc->wavetable->data[npos]; //lerp_sample >> 32;*/

  interp_set_config(interp0, 0, &oscilliator_global_lane0_config);
  interp_set_config(interp0, 1, &oscilliator_global_lane1_config);

  interp0->base[0] = osc->wavetable->data[npos];
  interp0->base[1] = osc->wavetable->data[next_npos];
  interp0->accum[1] = weight >> 8;

  osc->sample_pos += osc->increment;
  if (osc->sample_pos >= ((uint32_t)osc->wavetable->length << 16)) {
    osc->sample_pos -= (uint32_t)osc->wavetable->length << 16;
  }
  return interp0->peek[1];
}

#endif