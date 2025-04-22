add_executable(pico pico.c)

target_link_libraries(pico pico_stdlib hardware_i2c)
pico_add_extra_outputs(pico)