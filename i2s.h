#include <hardware/clocks.h>
#include <hardware/dma.h>
#include <hardware/pio.h>
#include <memory.h>
#include <pico/stdlib.h>

#include "i2s.pio.h"

#define I2S_BUFFER_SIZE 128

typedef void (*i2s_device_transfer_callback)(uint32_t *);
typedef struct {
  uint data_pin;
  uint clock_pin_base;

  uint pio_offset;
  uint pio_sm;

  int transfer_chan;
  i2s_device_transfer_callback transfer_callback;

  uint32_t buffer1[I2S_BUFFER_SIZE];
  uint32_t buffer2[I2S_BUFFER_SIZE];

} i2s_device;

i2s_device global_device;

void i2s_device_handle_dma() {
  memcpy(global_device.buffer2, global_device.buffer1,
         sizeof(uint32_t) * I2S_BUFFER_SIZE);
  dma_irqn_acknowledge_channel(0, global_device.transfer_chan);
  dma_channel_set_read_addr(global_device.transfer_chan, global_device.buffer2,
                            true);

  global_device.transfer_callback(global_device.buffer1);
}

void i2s_device_init(uint data_pin, uint clock_pin_base, float sample_rate,
                     i2s_device_transfer_callback transfer_callback) {
                      
  global_device.transfer_callback = transfer_callback;
  global_device.data_pin = data_pin;
  global_device.clock_pin_base = clock_pin_base;
  PIO pio = pio0;
  global_device.pio_offset = pio_add_program(pio, &i2s_program);
  global_device.pio_sm = pio_claim_unused_sm(pio, true);

  gpio_set_function(global_device.data_pin, GPIO_FUNC_PIO0);
  gpio_set_function(global_device.clock_pin_base, GPIO_FUNC_PIO0);
  gpio_set_function(global_device.clock_pin_base + 1, GPIO_FUNC_PIO0);

  global_device.transfer_chan = dma_claim_unused_channel(true);
  dma_channel_config transfer_chan_config =
      dma_channel_get_default_config(global_device.transfer_chan);
  channel_config_set_transfer_data_size(&transfer_chan_config, DMA_SIZE_32);
  channel_config_set_read_increment(&transfer_chan_config, true);
  channel_config_set_write_increment(&transfer_chan_config, false);
  channel_config_set_dreq(&transfer_chan_config,
                          pio_get_dreq(pio, global_device.pio_sm, true));
  dma_channel_configure(global_device.transfer_chan, &transfer_chan_config,
                        &pio->txf[global_device.pio_sm], NULL, I2S_BUFFER_SIZE,
                        false);

  i2s_program_init(pio, global_device.pio_sm, global_device.pio_offset,
                   global_device.data_pin, global_device.clock_pin_base);

  uint32_t clock_hz = clock_get_hz(clk_sys);
  float pio_clock_div = clock_hz / (sample_rate * 16 * 4);
  pio_sm_set_clkdiv(pio, global_device.pio_sm, pio_clock_div);
  pio_sm_set_enabled(pio, global_device.pio_sm, true);

  dma_channel_set_irq0_enabled(global_device.transfer_chan, true);
  irq_set_exclusive_handler(DMA_IRQ_0, i2s_device_handle_dma);
  irq_set_enabled(DMA_IRQ_0, true);

  for (int i = 0; i < I2S_BUFFER_SIZE; i++) {
    global_device.buffer1[i] = global_device.buffer2[i] = 0;
  }

  i2s_device_handle_dma();
}