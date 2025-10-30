#ifndef PICO_FLASH_HPP
#define PICO_FLASH_HPP
#include "pico/stdio.h"

uint32_t read_name_from_flash(char * buffer, uint maxlen);
uint32_t write_name_to_flash(char* buffer);

#endif