#include <string.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/util/queue.h" 
#include "pico/audio_i2s.h"

#include "audio.h"

static audio_format_t audio_format = {
    .sample_freq = 44100,
    .format = AUDIO_BUFFER_FORMAT_PCM_U8,
    .channel_count = 1,
};

#define AUD_DMA_CHANNEL 4 // requires 1 or 3 channels 

const struct audio_i2s_config config = {
    .data_pin = PICO_AUDIO_I2S_DATA_PIN,
    .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
    .dma_channel = AUD_DMA_CHANNEL,
    .pio_sm = 0,
};

static struct audio_buffer_format producer_format = {
    .format = &audio_format,
    .sample_stride = 1
};

#define SAMPLES_BUFFER_SIZE    2048 
queue_t audio_queue;

struct audio_buffer_pool *producer_pool; 

void audio_init(uint16_t sample_freq) {
    audio_format.sample_freq = sample_freq;

    const struct audio_format *output_format = audio_i2s_setup(&audio_format, &config);
    assert(output_format);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }

    producer_pool = audio_new_producer_pool(&producer_format, 3, 1); 
    if (!producer_pool) {
        panic("PicoAudio: Unable to allocate producer pool.\n");
    }

    bool ok = audio_i2s_connect(producer_pool);
    if (!ok) {
        panic("PicoAudio: Unable to connect producer to i2s\n");
    }



    audio_i2s_set_enabled(true);

}
void audio_early_init() {
    audio_dac_init();
    queue_init(&audio_queue, 1, SAMPLES_BUFFER_SIZE);
}

void audio_handle_buffer(void) {
    struct audio_buffer *buffer = take_audio_buffer(producer_pool, false);
    if (!buffer) return;

    uint32_t sample_count = 0, room = buffer->max_sample_count;
    uint8_t *ptr = buffer->buffer->bytes;
    while(room && queue_try_remove(&audio_queue, ptr)) {
        sample_count++;
        ptr++;
        room--;
    }
    buffer->sample_count = sample_count;
    give_audio_buffer(producer_pool, buffer);
}

void __not_in_flash_func(audio_push_sample)(const uint8_t sample) { 
    (void)queue_try_add(&audio_queue, &sample);
}
