#ifndef _WAVETABLE_H
#define _WAVETABLE_H

/*
 * Wavetable
 */
typedef struct {
  int16_t *data;
  uint32_t length;
} wavetable_t;

static inline wavetable_t wavetable_new(int16_t *data, uint32_t length) {
  wavetable_t wt;
  wt.data = data;
  wt.length = length;
  return wt;
}

/*
 * Presets
 */

int16_t __in_flash() wavetable_square_wave_data[64] = {
    0x0000, 0x47E7, 0x4C33, 0x4E00, 0x4F0A, 0x4FAF, 0x500B, 0x502E,
    0x5023, 0x4FF3, 0x4FA8, 0x4F4F, 0x4EF2, 0x4E9D, 0x4E59, 0x4E2D,
    0x4E1D, 0x4E2D, 0x4E59, 0x4E9D, 0x4EF2, 0x4F4F, 0x4FA8, 0x4FF3,
    0x5023, 0x502E, 0x500B, 0x4FAF, 0x4F0A, 0x4E00, 0x4C33, 0x47E7,
    0x0000, 0xB819, 0xB3CD, 0xB200, 0xB0F6, 0xB051, 0xAFF5, 0xAFD2,
    0xAFDD, 0xB00D, 0xB058, 0xB0B1, 0xB10E, 0xB163, 0xB1A7, 0xB1D3,
    0xB1E3, 0xB1D3, 0xB1A7, 0xB163, 0xB10E, 0xB0B1, 0xB058, 0xB00D,
    0xAFDD, 0xAFD2, 0xAFF5, 0xB051, 0xB0F6, 0xB200, 0xB3CD, 0xB819,
};

int16_t __in_flash() wavetable_sine_wave_data[64] = {
    0x0000, 0x0C8B, 0x18F8, 0x2527, 0x30FB, 0x3C56, 0x471C, 0x5133,
    0x5A81, 0x62F1, 0x6A6C, 0x70E1, 0x7640, 0x7A7C, 0x7D89, 0x7F61,
    0x7FFF, 0x7F61, 0x7D89, 0x7A7C, 0x7640, 0x70E1, 0x6A6C, 0x62F1,
    0x5A81, 0x5133, 0x471C, 0x3C56, 0x30FB, 0x2527, 0x18F8, 0x0C8B,
    0x0000, 0xF375, 0xE708, 0xDAD9, 0xCF05, 0xC3AA, 0xB8E4, 0xAECD,
    0xA57F, 0x9D0F, 0x9594, 0x8F1F, 0x89C0, 0x8584, 0x8277, 0x809F,
    0x8001, 0x809F, 0x8277, 0x8584, 0x89C0, 0x8F1F, 0x9594, 0x9D0F,
    0xA57F, 0xAECD, 0xB8E4, 0xC3AA, 0xCF05, 0xDAD9, 0xE708, 0xF375,
};

int16_t __in_flash() wavetable_sawtooth_wave_data[64] = {
    0x0000, 0x8EDF, 0xAD21, 0xA02F, 0xAE9D, 0xA8F0, 0xB31E, 0xB041,
    0xB86D, 0xB714, 0xBE0E, 0xBDAC, 0xC3DA, 0xC426, 0xC9BE, 0xCA8C,
    0xCFB1, 0xD0E7, 0xD5AD, 0xD73A, 0xDBB0, 0xDD87, 0xE1B8, 0xE3D0,
    0xE7C3, 0xEA16, 0xEDD1, 0xF05A, 0xF3E0, 0xF69D, 0xF9F0, 0xFCE0,
    0x0000, 0x0320, 0x0610, 0x0963, 0x0C20, 0x0FA6, 0x122F, 0x15EA,
    0x183D, 0x1C30, 0x1E48, 0x2279, 0x2450, 0x28C6, 0x2A53, 0x2F19,
    0x304F, 0x3574, 0x3642, 0x3BDA, 0x3C26, 0x4254, 0x41F2, 0x48EC,
    0x4793, 0x4FBF, 0x4CE2, 0x5710, 0x5163, 0x5FD1, 0x52DF, 0x7121,
};

wavetable_t wavetable_presets[] = {
    {.data = wavetable_square_wave_data, .length = 64},
    {.data = wavetable_sine_wave_data, .length = 64},
    {.data = wavetable_sawtooth_wave_data, .length = 64}};

#define WAVETABLE_PRESET_SQUARE &wavetable_presets[0]
#define WAVETABLE_PRESET_SINE &wavetable_presets[1]
#define WAVETABLE_PRESET_SAWTOOTH &wavetable_presets[2]

#endif