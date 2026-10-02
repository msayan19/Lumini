#pragma once

#include <stdint.h>

/**
 * @brief Opcode List
 * @note 16 opcodes for each group
 */
typedef enum : uint8_t {

	/* -------------------- Group 0: System & Data Access -------------------- */
	/* ------------------------------------------------------------------------ */
	OP_NOP = 	0U,		// no operation
	OP_HALT =	1U,		// stop execution
	OP_SYS = 	2U,		// do a syscall

	OP_LD = 	3U,		// load from heap
	OP_ST = 	4U,		// store to heap
	OP_LDI = 	5U,		// load a 16-bit immediate
	OP_CPY =	6U,		// copy register value
	OP_LDD = 	7U,		// load from static data

	
	/* ===================================================================== */
	/* -------------------- Group 1: Control Flow -------------------- */
	/* ------------------------------------------------------------------------ */

	OP_PUSH =	16U,	// push register value to stack
	OP_POP = 	17U,	// pop register value from stack
	OP_PUSHR =	18U,	// push sequential registers values to stack
	OP_POPR = 	19U,	// pop sequential registers values from stack

	OP_CALL = 	20U,	// call a subroutine
	OP_RET = 	21U,	// return from subroutine

	/**
	 * job: direct jump
	 */
	OP_JMP = 	22U,	// jump. address should be preloaded in reg[AD0]
	OP_IRJMP = 	23U,	// immediate relative jump. offset: 24-bits

	/** 
	 * Job: Compare & Jump Relatively (+-127) if condition is ture.
	 * offset: 8-bit (signed) - `RD`
	 * */
	OP_BGT = 	24U,	// branch if greater than. true condition: `reg[RS1] > reg[RS2]`
	OP_BLT = 	25U,	// branch if less than. true condition: `reg[RS1] < reg[RS2]`
	OP_BEQ = 	26U,	// branch if equal. true condition: `reg[RS1] == reg[RS2]`
	OP_BNE = 	27U,	// branch if not equal. true condition: `reg[RS1] != reg[RS2]`

	
	/* ===================================================================== */
	/* -------------------- Group 2: Bitwise Operations -------------------- */
	/* ------------------------------------------------------------------------ */

	OP_SHL = 	32U,	// bitwiseshift left.
	OP_SHR = 	33U,	// bitwise shift right 
	OP_ASR = 	34U,	// arithmetic shift right
	OP_AND = 	35U,	// bitwise and
	OP_OR = 	36U,	// bitwise or
	OP_NOT =	37U,	// bitwise not
	OP_XOR =	38U,	// bitwise xor

	/* ===================================================================== */
	/* -------------------- Group 3: Arithmetic -------------------- */
	/* ------------------------------------------------------------------------ */

	/* ---------- Signed Arithmetic ---------- */

	OP_ADD = 	48U,	// signed addition
	OP_SUB = 	49U,	// signed substraction
	OP_MUL = 	50U,	// signed multiplication
	OP_DIV = 	51U,	// signed divison
	OP_MOD = 	52U,	// signed modulus (remainder)
	/* ---------- Unsigned Arithmetic ---------- */

	OP_UADD = 	53U,	// unsigned addition
	OP_USUB = 	54U,	// unsigned substraction
	OP_UMUL = 	55U,	// unsigned multiplication
	OP_UDIV = 	56U,	// unsigned division
	OP_UMOD = 	57U,	// unsigned modulus (remainder)
	/* ---------- Floating Point Arithmetic ---------- */

	OP_FADD = 	58U,	// floating point addition
	OP_FSUB = 	59U,	// floating point substraction
	OP_FMUL = 	60U,	// floating point multiplication
	OP_FDIV = 	61U,	// floating point division
	OP_FMOD = 	62U,	// floating point modulus

	
	/* ===================================================================== */


} opcode_t;
