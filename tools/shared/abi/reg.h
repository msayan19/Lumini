#pragma once

#include <stdint.h>

union register_t {
	uint64_t u64;
	int64_t i64;
	double f64;
};

/* =====================================================================
				Register Configuration
========================================================================
*/



#define TOTAL_REGISTER_COUNT 256U
#define SPECIAL_REGISTER_COUNT 12U
#define GENERAL_REGISTER_COUNT 244U
#define ARGUMENTS_REGISTER_COUNT 16U
#define NON_VOLATILE_REGISTER_COUNT 72U
#define VOLATILE_REGISER_COUNT 156U

/*=====================================================================
					Register Index Mapping
=======================================================================
*/


/* ------------------------------------------ */
// Special Register Index Mapping
/* ------------------------------------------ */

/* Special Registers - read only */
#define REG_PC 255U		// program counter
#define REG_SR 254U		// status register
#define REG_SP 253U		// stack pointer
#define REG_CP 252U		// context pointer (currently unused)
/* Special Registers - read & write */
#define REG_HP 251U		// heap pointer
#define REG_AD 250U		// address regsiter
#define REG_SC 249U		// SysCall register
#define REG_TV 248U		// temporary value
/* Rserved Registers - read &  write */
#define REG_RSV0 247U	// reserved 0
#define REG_RSV1 246U	// reserved 1
#define REG_RSV2 245U	// reserved 2
#define REG_RSV3 244U	// reserved 3

/* ------------------------------------------- */

/* ------------------------------------------ */
// General Register Index Mapping
/* ------------------------------------------ */

#define MAX_REGISTER_INDEX (uint8_t)255U

#define ARGUMENT_REG_START 0
#define ARGUMENT_REG_END 15

#define NON_VOLATILE_REG_START 16
#define NON_VOLATILE_REG_END 87

#define VOLATILE_REG_START 88
#define VOLATILE_REG_END 243

/* ------------------------------------------ */
