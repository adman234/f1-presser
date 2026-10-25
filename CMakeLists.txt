cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico)
include(pico_sdk_import.cmake)

project(f1_keyboard C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

add_executable(f1_keyboard
    main.c
    usb_descriptors.c
)

target_include_directories(f1_keyboard PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(f1_keyboard PRIVATE
    pico_stdlib
    pico_unique_id
    tinyusb_device
)

pico_add_extra_outputs(f1_keyboard)
