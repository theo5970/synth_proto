#include <pico/platform.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "i2s.h"
#include "midi_message_queue.h"
#include "synth_machine.h"
#include "wavetable.h"

#define DATA_PIN 19        // DIN: GP19
#define CLOCK_PIN_BASE 20  // LRCLK: GP20 / BCLK: GP21
#define SAMPLE_RATE 44100

synth_machine sm;

void update_i2s_buffer(uint32_t *buffer) {
  synth_machine_update(&sm, buffer, I2S_BUFFER_SIZE);
}

void core1_entry() {

  while (!stdio_usb_connected()) {
    sleep_ms(100);
  }
  while (true) {
    int start_signal = getchar();
    if (start_signal != 0xAA) {
      continue;
    }

    midi_message_t msg;
    msg.status = (uint8_t)getchar();
    msg.data1 = (uint8_t)getchar();
    msg.data2 = (uint8_t)getchar();
    midi_message_queue_enqueue(&sm.midimsg_queue, msg);
  }
}

int main() {
  stdio_init_all();
  synth_machine_init(&sm);
  sm.sample_freq = SAMPLE_RATE;
  oscilliator_global_init();

  i2s_device_init(DATA_PIN, CLOCK_PIN_BASE, SAMPLE_RATE, update_i2s_buffer);
  multicore_launch_core1(core1_entry);
  while (true) {
    tight_loop_contents();
  }
}