#ifndef _ADSR_H
#define _ADSR_H

#include <hardware/divider.h>
#include <hardware/interp.h>
#include <pico/stdlib.h>

#define ADSR_STATE_ATTACK 0
#define ADSR_STATE_DECAY 1
#define ADSR_STATE_SUSTAIN 2
#define ADSR_STATE_RELEASE 3
#define ADSR_STATE_END 4

typedef struct {
  float attack;
  float decay;
  float sustain;
  float release;
} adsr_config;

adsr_config default_adsr_config = {
    .attack = 0.0f, .decay = 0.1f, .sustain = 0.6f, .release = 0.01f};

typedef struct {
  uint32_t timer;
  uint32_t attack_duration;
  uint32_t decay_duration;
  uint32_t sustain_level;
  uint32_t release_duration;
  uint16_t recent_output;
  uint8_t state;
} adsr_t;

static inline adsr_t adsr_create(adsr_config *config, uint32_t sample_rate) {
  adsr_t adsr;
  adsr.timer = 0;
  adsr.attack_duration = (uint32_t)(config->attack * sample_rate) + 1;
  adsr.decay_duration = (uint32_t)(config->decay * sample_rate) + 1;
  adsr.sustain_level = (uint32_t)(config->sustain * 65535);
  adsr.release_duration = (uint32_t)(config->release * sample_rate) + 1;
  adsr.state = ADSR_STATE_ATTACK;
  adsr.recent_output = 0;
  return adsr;
}

static inline void adsr_release(adsr_t *adsr) {
  adsr->timer = adsr->release_duration;

  if (adsr->state != ADSR_STATE_SUSTAIN) {
    adsr->sustain_level = adsr->recent_output;
  }
  adsr->state = ADSR_STATE_RELEASE;
}

static inline uint16_t adsr_update(adsr_t *adsr) {
  uint16_t ret = 0;
  switch (adsr->state) {
    case ADSR_STATE_ATTACK:
      adsr->timer++;
      if (adsr->timer >= adsr->attack_duration) {
        ret = 65535;
        adsr->timer = 0;
        adsr->state = ADSR_STATE_DECAY;
      } else {
        divmod_result_t div_result =
            hw_divider_divmod_u32(adsr->timer << 8, adsr->attack_duration);
        ret = (uint16_t)(to_quotient_u32(div_result) << 8);
      }
      break;
    case ADSR_STATE_DECAY:
      adsr->timer++;
      if (adsr->timer >= adsr->decay_duration) {
        ret = adsr->sustain_level;
        if (adsr->sustain_level == 0) {
          adsr_release(adsr);
        } else {
          adsr->timer = 0;
          adsr->state = ADSR_STATE_SUSTAIN;
        }
      } else {
        divmod_result_t div_result = hw_divider_divmod_u32(
            adsr->timer * (65535 - adsr->sustain_level), adsr->decay_duration);
        ret = 65535 - to_quotient_u32(div_result);
      }
      break;
    case ADSR_STATE_SUSTAIN:
      ret = adsr->sustain_level;
      break;
    case ADSR_STATE_RELEASE:
      adsr->timer--;
      if (adsr->timer == 0) {
        ret = 0;
        adsr->state = ADSR_STATE_END;
      } else {
        divmod_result_t div_result = hw_divider_divmod_u32(
            adsr->timer * adsr->sustain_level, adsr->release_duration);
        ret = to_quotient_u32(div_result);
      }
      break;
  }
  adsr->recent_output = ret;
  return ret;
}

#endif