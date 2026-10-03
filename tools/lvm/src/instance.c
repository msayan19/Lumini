#include "instance.h"
#include <string.h>

// create instance
instance_t create_instance(void) {
	instance_t i = {
		.state = LVM_INSTANCE_INITIALIZED,
		.heap = nullptr,
		.stack = nullptr,
		.data = nullptr,
		.program = nullptr,
		.heap_size = 0,
		.data_size = 0,
		.instruction_count = 0
	};
	/*set all register to 0 */
	memset(&i.reg,0,sizeof(i.reg));
	/* return */
	return i;
}

/**
 * @brief Setup the instance
 * @note do not free the contents of provided (lbp_file*) before the call of `destory_instance()`
 */
bool setup_instance(instance_t* instance,lbp_file* file) {
	/* nullptr check */
	if(instance == nullptr || file == nullptr) {
		return false;
	}
	/* Validate the provided file & its header 
	* this also validated the boundaries, program & data, file type etc. 
	so later code runs without any errors.
	*/
	if(!validate_lbp_file(file)) {
		instance->state = LVM_INSTANCE_SETUP_FAILED;
		return false;
	}
	// do not overwrite the ready to execute instance
	if(instance->state == LVM_INSTANCE_READY) {return false;}

	/* Setup Variables */
	instance->data_size = file->header.data_size;
	instance->instruction_count = file->header.instruction_count;
	instance->heap_size = file->header.heap_size;

	/* Borrow program & data */
	instance->program = file->program;
	if(file->data != nullptr) {
		instance->data = file->data;
	}
	/* Allocate Heap & Stack */
	/* if heap is requested, try to allocate it.*/
	if(file->header.heap_size > 0) {
		// allocate heap
		instance->heap = (uint8_t*)malloc(file->header.heap_size);
		// handle error for memory allocation.
		if(instance->heap == nullptr) {
			perror("Error: memory allocation for instance heap failed.");
			instance->program = nullptr;
			instance->data = nullptr;
			instance->state = LVM_INSTANCE_SETUP_FAILED;
			return false;
		}
	}

	/* Allocate the stack (the stack size is fixed at  8mb & won't change) */
	instance->stack = (uint8_t*)malloc(STACK_SIZE);
	// handle error for memory allocation
	if(instance->stack == nullptr) {
		free(instance->heap);
		instance->heap = nullptr;
		instance->program = nullptr;
		instance->data = nullptr;
		instance->state = LVM_INSTANCE_SETUP_FAILED;
		return false;
	}
	/* return */
	instance->state = LVM_INSTANCE_READY;
	return true;
}

// destroy instance
void destroy_instance(instance_t* instance) {
	/* check for nullpointers */
	if(instance == nullptr) {return;}
	/* set borrowed items to nullptr */
	instance->program = nullptr;
	instance->data = nullptr;
	/* free heap */
	if(instance->heap != nullptr) {
		free(instance->heap);
		instance->heap = nullptr;
	}
	/* free stack */
	if(instance->stack != nullptr) {
		free(instance->stack);
		instance->stack = nullptr;
	}
}