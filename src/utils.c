#include "utils.h"

int is_big_endian() {
    static uint32_t one = 1;
    return ((* (uint8_t *) &one) == 0);
}


void inline get_32B(uint32_t* bits, FILE* file) {
    fread(bits, sizeof(uint32_t), 1, file);
    if(!is_big_endian()) *bits = reverse_4(*bits);
}
void inline get_16B(uint16_t* bits, FILE* file){
    fread(bits, sizeof(uint16_t), 1, file);
    if(!is_big_endian()) *bits = reverse_2(*bits);
}
void inline get_8B(uint8_t* bits, FILE* file){
    fread(bits, sizeof(uint8_t), 1, file);
}

void inline write_32B(uint32_t* bits, FILE* file);
void inline write_16B(uint16_t* bits, FILE* file);
void inline write_8B(uint8_t* bits, FILE* file);