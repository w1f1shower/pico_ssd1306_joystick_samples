cmake_minimum_required(VERSION 3.13) 

include(pico_sdk_import.cmake)
project(pico_oled_joystick C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(
	main
        main.c
	ssd1306.c
        )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# pull in common dependencies and additional i2c hardware support
target_link_libraries(main pico_stdlib hardware_i2c hardware_adc pico_multicore)

pico_add_extra_outputs(main)
