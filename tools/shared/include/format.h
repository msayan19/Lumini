#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <zstd.h>
#include "core.h"
#include "wrappers.h"

#define LBP_IDENTIFIER_BYTE_COUNT 16
const uint8_t LBP_IDENTIFIER[LBP_IDENTIFIER_BYTE_COUNT] = {
    '_', 'l', 'u', 'm', 'i', 'n', 'i', '_', 
    'b', 'i', 'n', 'a', 'r', 'y', '_', '\0'
};

#define LBP_FILE_STATE_INITIALIZED  0x0
#define LPB_FILE_STATE_LOADED       0x1
#define LBP_FILE_STATE_UNLOADED     0x2

typedef struct {
	uint8_t identifier[LBP_IDENTIFIER_BYTE_COUNT];
    uint32_t abi_version;

	uint32_t heap_size;
    uint32_t instruction_count;
    uint32_t data_size;

    uint32_t flags;
    
    uint8_t padding[28];
} lbp_header;

#define LBP_HEADER_SIZE sizeof(lbp_header)

typedef struct {
    uint32_t status;
    lbp_header header;
    /* Binary Payload */
    uint8_t* program;
    uint8_t* data;
    /* ---------- */
} lbp_file;

/* Utility Functions */
bool validate_lbp_file(const lbp_file* f);
bool init_lbp_file(lbp_file* file, uint32_t abi_version, uint32_t instruction_count, uint32_t data_size, uint32_t heap_size, uint32_t flags);

/* Core functions */
bool load_lbp(lbp_file* file,const char* filepath);
bool write_lbp(const lbp_file* file,const char* filepath);
void free_lbp(lbp_file* file);

