/*
	File: isa.h
	Description: lvm ISA deifnitions
	Author: Sayan Malik (https://www.github.com/msayan19)
	Language: C (GNU C23)
	Dated: October 2026
	---------------------------------------------------------------------------------------
	NOTICE:

	Copyright 2026 Sayan Malik

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

	==========================================================================================
*/

#pragma once
#include <stdint.h>

#ifndef ISA_H
#define ISA_H 1U		// fallback to ISA v1.0
#endif



#define REGISTER_COUNT 256UL
#define RESERVED_REGS 8U	// total reserved registers
#define REG_FL 0U			// Flags Register
#define REG_PC 1U			// Program Counter
#define REG_SP 2U			// Stack Pointer
#define REG_MA 3U			// Memory Address
#define REG_TV 4U			// Temporary Value
#define REG_R0 5U			// Reserved 0
#define REG_R1 6U			// Reserved 1
#define REG_R2 7U			// Reserved 2


typedef union {
	uint64_t u64;
	int64_t i64;
	double f64;
} reg_t;

typedef union {
	uint32_t raw;				// raw instruction

	struct {
		uint32_t operation : 8; // Operation
		uint32_t a : 8;      	// operand A
		uint32_t b : 8;      	// operand B
		uint32_t c : 8;      	// operand C
	} rtype;

	struct {
		uint32_t operation : 8; // Operation
		uint32_t a : 8;      	// operand A
		uint32_t imm16 : 16; 	// 16-bit Immediate
	} itype;

	struct {
		uint32_t operation : 8; // Operation
	   uint32_t imm24: 24;   	// 24-bit Immediate
	} jtype;
} instr_t;


typedef enum : uint8_t {
	// Group 0 (0 - 15): System & Memory Operations
	OP_STOP		= 0U,		// STOP execution
	OP_SYS		= 1U,		// Syscall

	OP_LDS		= 2U,		// Load Signed
	OP_LDU		= 3U,		// Load Unsigned
	OP_LDF		= 4U,		// Load Float
	OP_STS		= 5U,		// Store Signed
	OP_STU		= 6U,		// Store Unsigned
	OP_STF		= 7U,		// Store Float
	OP_CPY		= 8U,		// Copy
	
	OP_LIS8		= 9U,		// Load Immediate Signed 8-bit
	OP_LIS16	= 10U,		// Load Immediate Signed 16-bit
	OP_LIU8		= 11U,		// Load Immediate Unsigned 8-bit
	OP_LIU16	= 12U,		// Load Immediate Unsigned 16-bit

	OP_CRV		= 13U,		// Clear Register Value (set all bits to 0)

	// Group 1 (16 - 31): Arithmetic Operations
	OP_ADDS		= 16U,		// Add Signed 
	OP_SUBS		= 17U,		// Substract Signed
	OP_MULS		= 18U,		// Multiply Signed
	OP_DIVS		= 19U,		// Divide Signed
	OP_REMS		= 20U,		// Remainder Signed

	OP_ADDU		= 21U,		// Add Unsigned
	OP_SUBU		= 22U,		// Substract Unsigned
	OP_MULU		= 23U,		// Multiply Unsigned
	OP_DIVU		= 24U,		// Divide Unsigned
	OP_REMU		= 25U,		// Remainder Unsigned

	OP_ADDF		= 26U,		// Add Float
	OP_SUBF		= 27U,		// Substract Float
	OP_MULF		= 28U,		// Multiply Float
	OP_DIVF		= 29U,		// Divide Float
	OP_REMF		= 30U,		// Remainder Float
	// Group 2 (32 - 47): Bitwise Operations
	OP_ASL		= 32U,		// Arithmetic Shift Left
	OP_ASR		= 33U,		// Arithmetic Shift Right
	OP_SL		= 34U,		// Shift Left
	OP_SR		= 35U,		// Shift Right
	OP_AND		= 36U,		// bitwise AND
	OP_OR		= 37U,		// bitwise OR
	OP_NOT		= 38U,		// bitwise NOT
	OP_XOR		= 39U,		// bitwise XOR
	// Group 3 & 4 (48 - 79): Comparison & Control Flow Operations (Branch, Jump, Stack, Subroutine) 
	OP_ES		= 48U,		// Equal Signed
	OP_GS		= 49U,		// Greater Signed
	OP_LS		= 50U,		// Less Signed

	OP_EU		= 51U,		// Equal Unsigned
	OP_GU		= 52U,		// Greater Unsigned
	OP_LU		= 53U,		// Less Unsigned

	OP_EF		= 54U,		// Equal Float
	OP_GF		= 55U,		// Greater Float
	OP_LF		= 56U,		// Less Float
	OP_CF		= 57U,		// Classify Float

	OP_BEC		= 58U,		// Branch on Equal Conditions
	OP_BUC		= 59U,		// Branch on Unequal Conditions

	OP_BE		= 60U,		// Branch if Equal
	OP_BU		= 61U,		// Branch if Unequal
	OP_BG		= 62U,		// Branch if Greater
	OP_BL		= 63U,		// Branch if Less
	OP_BGE		= 64U,		// Branch if Greater or Equal
	OP_BLE		= 65U,		// Branch if Less or Equal
	OP_BNAN		= 66U,		// Branch if NAN (Not A Number)
	OP_BINF		= 67U,		// Branch if INF (Infinity)
	OP_BERR		= 68U,		// Branch on Error

	OP_JD		= 69U,		// Jump Direct
	OP_JI		= 70U,		// Jump Indirect
	OP_JR		= 71U,		// Jump Relative
	OP_JIR		= 72U,		// Jump Indirect Register
	OP_JRR		= 73U,		// Jump Relative Register

	OP_PUSH		= 74U,		// Push
	OP_POP		= 75U,		// Pop
	OP_PUSHM	= 76U,		// Push Multiple
	OP_POPM		= 77U,		// Pop Multiple
	OP_CALL		= 78U,		// Call
	OP_RET		= 79U,		// Return
} opcode_t;