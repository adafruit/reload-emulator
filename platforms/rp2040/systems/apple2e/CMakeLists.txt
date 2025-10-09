# Replace TMDS with 10 bit UART (same baud rate):
# add_definitions(-DDVI_SERIAL_DEBUG=1)
# add_definitions(-DRUN_FROM_CRYSTAL)

add_executable(apple2e
	${CMAKE_CURRENT_SOURCE_DIR}/src/apple2e.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils.S
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/hid_app.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/audio.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/audio_init.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../lib/sd_driver/tf_card.c
)

target_compile_options(apple2e PRIVATE -Wall)

target_link_libraries(apple2e
        pico_stdlib
        pico_multicore
        pico_util
        tinyusb_host tinyusb_pico_pio_usb
        hardware_interp
        hardware_pwm
        hardware_spi
        fatfs
        pico_util_buffer
        pico_audio
        pico_audio_i2s
        hardware_i2c
)

# create map/bin/hex file etc.
pico_add_extra_outputs(apple2e)

pico_enable_stdio_usb(apple2e 0)
pico_enable_stdio_uart(apple2e 1)
