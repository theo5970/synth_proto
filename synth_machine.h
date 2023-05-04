#ifndef _SYNTH_MACHINE_H
#define _SYNTH_MACHINE_H

#include "adsr.h"
#include "midi_message_queue.h"
#include "oscilliator.h"
#include "preset.h"

#define SYNTH_MACHINE_MAX_VOICES 12

// Velocity to Amplitude Table
// Fixed Point (Fraction: 16-bit)
uint16_t __in_flash() velamp_table[128] = {
    0x0000, 0x0085, 0x010C, 0x0197, 0x0223, 0x02B2, 0x0344, 0x03D9, 0x0470,
    0x050A, 0x05A7, 0x0647, 0x06E9, 0x078F, 0x0838, 0x08E3, 0x0992, 0x0A44,
    0x0AFA, 0x0BB2, 0x0C6E, 0x0D2E, 0x0DF1, 0x0EB7, 0x0F81, 0x104F, 0x1121,
    0x11F6, 0x12D0, 0x13AD, 0x148E, 0x1574, 0x165D, 0x174B, 0x183E, 0x1935,
    0x1A30, 0x1B30, 0x1C34, 0x1D3E, 0x1E4C, 0x1F5F, 0x2077, 0x2194, 0x22B7,
    0x23DF, 0x250C, 0x263F, 0x2777, 0x28B5, 0x29F9, 0x2B43, 0x2C93, 0x2DE9,
    0x2F45, 0x30A8, 0x3211, 0x3381, 0x34F7, 0x3674, 0x37F9, 0x3984, 0x3B17,
    0x3CB1, 0x3E52, 0x3FFB, 0x41AC, 0x4365, 0x4526, 0x46EF, 0x48C1, 0x4A9B,
    0x4C7D, 0x4E69, 0x505D, 0x525B, 0x5462, 0x5672, 0x588C, 0x5AB0, 0x5CDE,
    0x5F17, 0x6159, 0x63A6, 0x65FE, 0x6861, 0x6ACF, 0x6D49, 0x6FCE, 0x725F,
    0x74FC, 0x77A5, 0x7A5B, 0x7D1D, 0x7FEC, 0x82C8, 0x85B2, 0x88AA, 0x8BAF,
    0x8EC3, 0x91E4, 0x9515, 0x9854, 0x9BA3, 0x9F01, 0xA26F, 0xA5ED, 0xA97C,
    0xAD1B, 0xB0CB, 0xB48C, 0xB85F, 0xBC44, 0xC03B, 0xC444, 0xC861, 0xCC91,
    0xD0D4, 0xD52B, 0xD997, 0xDE17, 0xE2AD, 0xE758, 0xEC19, 0xF0F0, 0xF5DD,
    0xFAE2, 0xFFFF};

// MIDI Key to Frequency Table
// Base: 440Hz
// Fixed Point (Integer: 16-bit, Fraction: 16-bit)
uint32_t __in_flash() key_freq_table[128] = {
    0x00082CF8, 0x0008A96D, 0x00092D48, 0x0009B8FA, 0x000A4CFB, 0x000AE9C8,
    0x000B8FE9, 0x000C3FEA, 0x000CFA62, 0x000DBFF2, 0x000E9141, 0x000F6F01,
    0x001059F1, 0x001152DA, 0x00125A90, 0x001371F4, 0x001499F6, 0x0015D390,
    0x00171FD2, 0x00187FD5, 0x0019F4C5, 0x001B7FE4, 0x001D2281, 0x001EDE03,
    0x0020B3E3, 0x0022A5B5, 0x0024B521, 0x0026E3E9, 0x002933EB, 0x002BA722,
    0x002E3FA4, 0x0030FFA9, 0x0033E98C, 0x0036FFC9, 0x003A4503, 0x003DBC06,
    0x004167C7, 0x00454B6A, 0x00496A42, 0x004DC7D2, 0x005267D7, 0x00574E44,
    0x005C7F48, 0x0061FF52, 0x0067D318, 0x006DFF92, 0x00748A07, 0x007B780C,
    0x0082CF8F, 0x008A96D6, 0x0092D484, 0x009B8FA5, 0x00A4CFAF, 0x00AE9C88,
    0x00B8FE90, 0x00C3FEA7, 0x00CFA630, 0x00DBFF24, 0x00E9140C, 0x00F6F019,
    0x01059F1E, 0x01152DAC, 0x0125A908, 0x01371F4A, 0x01499F5E, 0x015D3910,
    0x0171FD20, 0x0187FD4C, 0x019F4C60, 0x01B7FE48, 0x01D2281C, 0x01EDE032,
    0x020B3E3C, 0x022A5B58, 0x024B5210, 0x026E3E94, 0x02933EBC, 0x02BA7220,
    0x02E3FA40, 0x030FFA98, 0x033E98C0, 0x036FFC90, 0x03A45038, 0x03DBC064,
    0x04167C78, 0x0454B6B0, 0x0496A420, 0x04DC7D28, 0x05267D78, 0x0574E440,
    0x05C7F480, 0x061FF538, 0x067D3180, 0x06DFF920, 0x0748A060, 0x07B780D8,
    0x082CF8F0, 0x08A96D50, 0x092D4850, 0x09B8FA50, 0x0A4CFAE0, 0x0AE9C880,
    0x0B8FE900, 0x0C3FEA50, 0x0CFA6310, 0x0DBFF240, 0x0E9140C0, 0x0F6F01B0,
    0x1059F1E0, 0x1152DAA0, 0x125A90A0, 0x1371F4A0, 0x1499F5C0, 0x15D39100,
    0x171FD200, 0x187FD4A0, 0x19F4C620, 0x1B7FE480, 0x1D228200, 0x1EDE02E0,
    0x20B3E3C0, 0x22A5B580, 0x24B520C0, 0x26E3E940, 0x2933EC00, 0x2BA721C0,
    0x2E3FA400, 0x30FFAA00,
};

/*
 * Synth Voice
 */
typedef struct {
  oscilliator main_osc;
  oscilliator lfo_osc;
  uint8_t key;
  uint8_t velocity;
  adsr_t adsr;
} synth_voice;

/*
 * Synth Machine
 */
typedef struct {
  uint32_t sample_freq;
  synth_voice voices[SYNTH_MACHINE_MAX_VOICES];
  uint32_t count_voices;

  midi_message_queue midimsg_queue;
  // int8_t key_voice_map[128];
  synth_preset presets[2];

} synth_machine;

static inline void synth_machine_init(synth_machine *sm) {
  sm->count_voices = 0;
  for (int i = 0; i < SYNTH_MACHINE_MAX_VOICES; i++) {
    synth_voice *voice = &sm->voices[i];
    voice->main_osc = oscilliator_get_default();
    voice->velocity = 0;
  }

  sm->presets[0] = default_synth_preset1;
  sm->presets[1] = default_synth_preset2;

  /*for (int i = 0; i < 128; i++) {
    sm->key_voice_map[i] = -1;
  }*/
}

static inline void synth_machine_add_voice(synth_machine *sm,
                                           synth_voice voice) {
  if (sm->count_voices >= SYNTH_MACHINE_MAX_VOICES) {
    return;
  }

  sm->voices[sm->count_voices++] = voice;
  // sm->key_voice_map[voice.key] = sm->count_voices - 1;
}

static inline void synth_machine_remove_voice(synth_machine *sm,
                                              int voice_index) {
  uint8_t key = sm->voices[voice_index].key;
  int shift_count = sm->count_voices - voice_index - 1;
  for (int i = 0; i < shift_count; i++) {
    int current_index = voice_index + i;
    int next_index = voice_index + i + 1;
    // sm->key_voice_map[sm->voices[next_index].key]--;
    sm->voices[current_index] = sm->voices[next_index];
  }
  sm->count_voices--;
  // sm->key_voice_map[key] = -1;
}

static inline void synth_machine_note_on(synth_machine *sm, uint8_t channel, uint8_t key,
                                         uint8_t velocity) {
  if (sm->count_voices >= SYNTH_MACHINE_MAX_VOICES) {
    synth_machine_remove_voice(sm, 0);
  }

  synth_preset *preset = &sm->presets[channel];
  synth_voice new_voice;
  new_voice.key = key;
  new_voice.velocity = velocity;
  new_voice.adsr = adsr_create(&preset->adsr_config, sm->sample_freq);

  oscilliator *main_osc = &new_voice.main_osc;
  main_osc->wavetable = &wavetable_presets[rand() % 3];
  main_osc->sample_pos = 0;
  main_osc->increment =
      (uint32_t)(((uint64_t)key_freq_table[key] * main_osc->wavetable->length) /
                 sm->sample_freq);  // / 468;

 
  synth_machine_add_voice(sm, new_voice);
}

static inline void synth_machine_note_off(synth_machine *sm, uint8_t key) {
  for (int i = 0; i < sm->count_voices; i++) {
    synth_voice *voice = &sm->voices[i];
    if (voice->key == key && voice->adsr.state != ADSR_STATE_RELEASE) {
      adsr_release(&voice->adsr);
    }
  }
}

static inline void synth_machine_update(synth_machine *sm, uint32_t *buffer,
                                        uint32_t buffer_length) {
  while (!midi_message_queue_is_empty(&sm->midimsg_queue)) {
    midi_message_t msg;
    midi_message_queue_dequeue(&sm->midimsg_queue, &msg);

    if ((msg.status >> 4) == 9) {
      if (msg.data2 > 0) {
        synth_machine_note_on(sm, 0, msg.data1, msg.data2);
      } else {
        synth_machine_note_off(sm, msg.data1);
      }
    } else if ((msg.status >> 4) == 8) {
      synth_machine_note_off(sm, msg.data1);
    }
  }
  for (int i = 0; i < buffer_length; i++) {
    int16_t output = 0;
    for (int k = 0; k < sm->count_voices; k++) {
      synth_voice *voice = &sm->voices[k];

      int16_t amp = (int16_t)(((int32_t)oscilliator_update(&voice->main_osc) *
                               velamp_table[voice->velocity]) >>
                              16);
      amp = (int16_t)(((int32_t)amp * adsr_update(&voice->adsr)) >> 16);
      output += amp >> 2;

      if (voice->adsr.state == ADSR_STATE_END) {
        synth_machine_remove_voice(sm, k--);
      }
    }

    buffer[i] = (uint16_t)output | ((uint16_t)output << 16);
  }
}

#endif
