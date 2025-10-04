#ifndef FRUITJAM_CFLAGS_H
#define FRUITJAM_CFLAGS_H 1
#define PICO_DEFAULT_UART 0
#define PICO_DEFAULT_UART_TX_PIN 44
#define PICO_DEFAULT_UART_RX_PIN 45
#define PICO_AUDIO_I2S_DATA_PIN 24
#define PICO_AUDIO_I2S_CLOCK_PIN_BASE 26

#define HSTX_CKP 13
#define HSTX_D0P 15
#define HSTX_D1P 17
#define HSTX_D2P 19

#define HAS_USBPIO
#define PIN_USB_HOST_DP (1u)
#define PIN_USB_HOST_DM (2u)
#define PIN_USB_HOST_VBUS (11u)

// Note that by good luck (??) olimex_neo6502_cfg may match fruitjam
#endif
