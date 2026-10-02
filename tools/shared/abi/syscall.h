#pragma once

#include <stdint.h>

/**
 * @brief Syscall Codes list
 * @note Codes must be saved to reg[SC] before doing a syscall
 */
typedef enum : uint16_t {
	SYSC_TIME = 	0U,		// get time in reg[RD]
	SYSC_COUT = 	1U,		// output reg[RD]. RS1 = type
	SYSC_CIN = 		2U,		// input & store to reg[RD]. RS1 = type
} syscall_code_t;

/**
 * @brief Syscall Console I/O Types
 * @note this is used in both SYSC_CIN & SYSC_COUT.
 */
typedef enum : uint8_t {
	SYSC_CIO_CHAR = 0U,		// Character
	SYSC_CIO_INT = 	1U,		// Signed Integer
	SYSC_CIO_UINT = 2U,		// Unsigned Integer
	SYSC_CIO_FLOAT = 3U,	// Float
	SYSC_CIO_HEX = 	4U,		// Hex
	/* Note: reg[RD] ignored here */
	SYSC_CIO_STRING = 5U,	// String. start address: reg[AD0] & size: reg[TV]
} syscall_cio_type_t;
