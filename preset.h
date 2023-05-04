#ifndef _PRESET_H
#define _PRESET_H

#include "adsr.h"
#include "wavetable.h"

typedef struct {
    adsr_config adsr_config;
    wavetable_t *wavetable;
} synth_preset;


synth_preset default_synth_preset1 = {
    .adsr_config = {
        .attack = 0.0f,
        .decay = 0.1f,
        .sustain = 0.7f,
        .release = 0.01f
    },
    .wavetable = WAVETABLE_PRESET_SAWTOOTH
};

synth_preset default_synth_preset2 = {
    .adsr_config = {
        .attack = 0.0f,
        .decay = 0.1f,
        .sustain = 0.65f,
        .release = 0.01f
    },
    .wavetable = WAVETABLE_PRESET_SAWTOOTH
};
#endif