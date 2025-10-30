#include "pico_flash.hpp"
#include "pico/flash.h"
#include "hardware/flash.h"
#include "string.h"




struct flash_erase_param {
    uint32_t offset;
    uint32_t size;
};

struct flash_program_params {
    uint32_t flash_offset;
    uint32_t size;
    uint8_t * data_ptr;
};
// This function will be called when it's safe to call flash_range_erase
static void call_flash_sector_erase(void *param) {
    uint32_t offset = ((struct flash_erase_param *)param)->offset;
    uint32_t size = ((struct flash_erase_param*)param)->size;
    flash_range_erase(offset, FLASH_SECTOR_SIZE*size);
}

static void call_flash_page_program(void *param) {
    uint32_t offset = ((struct flash_program_params *)param)->flash_offset;
    uint32_t size = ((struct flash_program_params *)param)->size;
    uint8_t *data_ptr = ((struct flash_program_params *)param)->data_ptr;
    flash_range_program(offset, data_ptr, FLASH_PAGE_SIZE*size);
}


inline uint32_t *getAddressPersistent() {
extern uint32_t ADDR_PERSISTENT[];
    return ADDR_PERSISTENT;
}
uint32_t read_name_from_flash(char * buffer, uint maxlen) {
    if(((uint8_t*)getAddressPersistent())[0] == 0xFF){
        return 0;
    }
    memccpy(buffer, ((uint8_t*)getAddressPersistent()), 0,maxlen);
    return strlen(buffer);
}

uint32_t write_name_to_flash(char* buffer) {
    uint8_t write_buff[256] = {0x00};
    memccpy(write_buff,buffer,0,255);
    write_buff[255] = 0;
    struct flash_erase_param erase_params = { .offset = (uint32_t)getAddressPersistent()-0x10000000, .size = 1};
    int rc = flash_safe_execute(call_flash_sector_erase, &erase_params, UINT32_MAX);
    if(rc) return rc;
    struct flash_program_params prog_params = {.flash_offset = (uint32_t)getAddressPersistent()-0x10000000, .size = 1, .data_ptr = write_buff};
    rc = flash_safe_execute(call_flash_page_program, &prog_params, UINT32_MAX);
    return rc;
}
