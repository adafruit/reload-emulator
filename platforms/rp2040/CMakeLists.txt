cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/pico-sdk)
set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/tinyusb)
set(PICO_PIO_USB_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/Pico-PIO-USB) 

include(pico_sdk_import.cmake)

project(rp2040 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD adafruit_fruit_jam)
# set(PICO_BOARD olimex_rp2040pc)
# set(PICO_BOARD pico_zero)

pico_sdk_init()

add_compile_options(-Wall)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/lib/fatfs/source
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)

add_subdirectory(lib/pico-extras/src/rp2_common/pico_audio_i2s)
add_subdirectory(lib/pico-extras/src/common/pico_audio)
add_subdirectory(lib/pico-extras/src/common/pico_util_buffer)

add_subdirectory(lib/fatfs/source)
add_subdirectory(systems)
