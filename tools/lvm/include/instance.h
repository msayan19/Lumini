#pragma once

#include <stdint.h>
#include "abi.h"
#include "format.h"

#define LVM_INSTANCE_INITIALIZED 0x0U
#define LVM_INSTANCE_READY 0x1U
#define LVM_INSTANCE_FREED 0x2U

#define LVM_INSTANCE_SETUP_FAILED 0x03

typedef struct {
	union register_t reg[TOTAL_REGISTER_COUNT];
	uint8_t* program;
	uint8_t* stack;
	uint8_t* heap;
	uint8_t* data;

	uint32_t instruction_count;
	uint32_t heap_size;
	uint32_t data_size;
	uint32_t state;
} instance_t;

/**
 * @brief create an instance object
 * @return Initialized instance object
 */
instance_t create_instance(void);
/**
 * @brief set the up instance object
 * 
 * @param instance instance object to setup
 * @param file lbp file object to borrow data & program
 * @note do not free the contents of provided (lbp_file*) before the call of `destory_instance()`
 * @return true on success
 * @return false on failure
 */
bool setup_instance(instance_t* instance,lbp_file* file);
/**
 * @brief destroy instance object
 * @note sets borrowed items (program & data) to nullpointer
 * @note frees stack & heap
 */
void destroy_instance(instance_t* instance);