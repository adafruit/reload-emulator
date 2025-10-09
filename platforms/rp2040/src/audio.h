#ifndef AUDIO_H_FILE
#define AUDIO_H_FILE

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OLIMEX_NEO6502) || defined(OLIMEX_RP2040PC)
#define _AUDIO_PIN (20)
#else
#define _AUDIO_PIN (8)
#endif

void audio_early_init();
void audio_init(uint16_t sample_freq); // has to happen on the core that'll handle audio interrupts
void audio_push_sample(const uint8_t sample);
void audio_dac_init(void);
void audio_handle_buffer(void);

#ifdef __cplusplus
}
#endif

#endif  // AUDIO_H_FILE
