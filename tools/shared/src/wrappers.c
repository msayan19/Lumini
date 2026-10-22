#include "wrappers.h"
#include <stdint.h>

bool is_system_little_endian() {
    uint16_t number = 0x0001;
    uint8_t *first_byte = (uint8_t *)&number;
    if (*first_byte == 0x01) {
        return true;
    } else {
        return false;
    }
    return true;
}