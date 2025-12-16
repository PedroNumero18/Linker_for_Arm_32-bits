#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdint.h>

//pris depuis le util.h donnée depuis elf_linker-1.0

#define reverse_2(x) ((((x)&0xFF)<<8)|(((x)>>8)&0xFF))
#define reverse_4(x) ((((x)&0xFF)<<24)|((((x)>>8)&0xFF)<<16)|((((x)>>16)&0xFF)<<8)|(((x)>>24)&0xFF))

void get_32B(uint32_t* bits, FILE* file);
void get_16B(uint16_t* bits, FILE* file);
void get_8B(uint8_t* bits, FILE* file);

void write_32B(uint32_t* bits, FILE* file);
void write_16B(uint16_t* bits, FILE* file);
void write_8B(uint8_t* bits, FILE* file);

#endif