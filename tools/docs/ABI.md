# LVM Application Binary Interface (ABI) Specification

| | |
| --- | --- |
| **Version** | 1.0 |
| **ISA Version** | 1.0 |
| **Language Standard Compatibility** | C (GNU C23) |
| **Date** | October 2026 |
| **Source Headers** | `abi.h`, `isa.h` |
| **Author/Maintainer** | Sayan Malik (<https://www.github.com/msayan19>) |

## 1. Overview

This document specifies the Application Binary Interface (ABI) v1.0 for the Lightweight Virtual Machine (LVM). The ABI defines the low-level machine interface between application programs, libraries, and the underlying virtual execution environment. It establishes structural standards for register allocations, calling conventions, stack setups, and system interactions based on the LVM 32-bit fixed-width Instruction Set Architecture (ISA v1.0).

**Versioning.** If `ABI_VERSION` is not defined, `abi.h` falls back to ABI v1.0 by defining `ABI_VERSION 1` and `ISA_VERSION 1` (ABI v1.0 uses ISA v1.0). `abi.h` includes `isa.h`.

## 2. Register Files & Layout

The LVM architecture exposes a total of 256 universal registers (`REGISTER_COUNT 256UL`), each treated as a uniform unit for address mapping or raw values. Every register is 64 bits wide, represented by the `reg_t` union (`u64` unsigned, `i64` signed, `f64` double-precision float). The register space is rigidly divided into standard control blocks, argument channels, and execution volatility zones.

### 2.1 Special & Reserved Registers

The first 8 registers (`RESERVED_REGS 8U`, addresses 0 through 7) are dedicated to execution control and machine state management. They cannot be used as scratch registers or general allocation points.

| Register Identifier | Address (Dec / Hex) | Purpose / Architectural Description |
| --- | --- | --- |
| `REG_FL` | 0U / 0x00 | **Flags Register:** Captures arithmetic status, floating-point classifications, error states, and comparison results. |
| `REG_PC` | 1U / 0x01 | **Program Counter:** Points to the memory address of the instruction currently being fetched/executed. |
| `REG_SP` | 2U / 0x02 | **Stack Pointer:** Tracks the top of the runtime stack for frame manipulation, push/pop ops, and subroutine links. |
| `REG_MA` | 3U / 0x03 | **Memory Address:** Intermediate or tracking register for explicit data load/store memory operations. |
| `REG_TV` | 4U / 0x04 | **Temporary Value:** Internal machine scratch register reserved for intermediate operation mechanics. |
| `REG_R0` | 5U / 0x05 | **Reserved 0:** System/hardware reserved slot for subsequent extension tiers. |
| `REG_R1` | 6U / 0x06 | **Reserved 1:** System/hardware reserved slot for subsequent extension tiers. |
| `REG_R2` | 7U / 0x07 | **Reserved 2:** System/hardware reserved slot for subsequent extension tiers. |

### 2.2 General-Purpose Register Allocations

Outside the reserved block, registers 8 through 255 are mapped by the ABI into functional roles governing data longevity across function boundaries.

- **Argument Registers** (`ARGUMENT_REGISTERS = 16U`)
  - **Range:** Registers 8U to 23U (0x08 – 0x17), defined as `ARGUMENT_REGISTER_START` / `ARGUMENT_REGISTER_END`
  - **Role:** Used for passing incoming function parameters and returning calculated result structures.
- **Volatile / Caller-Saved Registers** (`VOLATILE_REGISTERS = 124U`)
  - **Range:** Registers 24U to 131U (0x18 – 0x83), defined as `VOLATILE_REGISTER_START` / `VOLATILE_REGISTER_END`
  - **Role:** Scratch registers available for heavy computation inside functions. Subroutines may overwrite these freely. The caller must explicitly preserve them if their lifetime spans an `OP_CALL`.
- **Non-Volatile / Callee-Saved Registers** (`NON_VOLATILE_REGISTERS = 124U`)
  - **Range:** Registers 132U to 255U (0x84 – 0xFF), defined as `NON_VOLATILE_REGISTER_START` / `NON_VOLATILE_REGISTER_END`
  - **Role:** Long-term storage registers. The callee (subroutine) must preserve these values before using them (typically via `OP_PUSH` or `OP_PUSHM`) and restore them (`OP_POP` or `OP_POPM`) prior to issuing an `OP_RET`.

## 3. Instruction Encoding Layout

LVM instructions are standardized as uniform 32-bit (`uint32_t raw`) words. Depending on the operational behavior, they follow one of three precise binary layouts. In `isa.h` these are bitfield structs (`rtype`, `itype`, `jtype`) inside the `instr_t` union, alongside the `raw` 32-bit word.

### 3.1 R-Type (Register Type)

Used for operations requiring pure register interaction, such as standard three-operand triadic math.

| Bits | Field | Width | Description |
| --- | --- | --- | --- |
| 0 – 7 | `operation` | 8 bits | Opcode identifying the execution routine. |
| 8 – 15 | `a` | 8 bits | Target register operand A. |
| 16 – 23 | `b` | 8 bits | Source register operand B. |
| 24 – 31 | `c` | 8 bits | Source register operand C. |

### 3.2 I-Type (Immediate Type)

Used for operations binding an immediate constant value to a target register destination.

| Bits | Field | Width | Description |
| --- | --- | --- | --- |
| 0 – 7 | `operation` | 8 bits | Opcode identifying the execution routine. |
| 8 – 15 | `a` | 8 bits | Destination register operand A. |
| 16 – 31 | `imm16` | 16 bits | Literal immediate value (signed/unsigned depending on opcode context). |

### 3.3 J-Type (Jump/Global Type)

Used for wide-range control-flow operations such as global direct jumps.

| Bits | Field | Width | Description |
| --- | --- | --- | --- |
| 0 – 7 | `operation` | 8 bits | Opcode identifying the jump/control routine. |
| 8 – 31 | `imm24` | 24 bits | Broad immediate constant offset or absolute target. |

## 4. Calling Conventions & Stack Protocol

### 4.1 Parameter Passing

- Up to 16 native scalar or pointer variables are placed directly in the argument register block sequentially (REG 8 → REG 23).
- If an activation record demands more than 16 words, additional arguments must be pushed onto the execution stack in reverse order (right-to-left) prior to issuing `OP_CALL`.

### 4.2 Return Mechanisms

- Scalar outputs, reference values, or exit states are written to the lower argument bounds (starting at REG 8).
- Complex or wide structure responses exceeding standard register bounds should be passed by reference, pointing to a dedicated buffer allocated by the caller in memory.

### 4.3 Stack Frame Mechanics

- The `REG_SP` (Stack Pointer) register tracks the active stack frame boundary. The stack grows downwards towards lower memory addresses.
- Alignment rules require stack preservation thresholds corresponding to standard 32-bit architectural granularities.
- Multi-register preservation must favor layout structures natively parsed by `OP_PUSHM` and `OP_POPM`.

## 5. ISA Opcode Matrix & Classification

The runtime maps behavior according to the functional groupings detailed in the tables below. Opcodes are 8-bit (`opcode_t`, an enum based on `uint8_t`). Values not listed (14–15, 31, 40–47) are currently unassigned.

### 5.1 Group 0 (0 – 15): System & Memory Operations

| Opcode Value | Mnemonic | Format Type | Description |
| --- | --- | --- | --- |
| 0U | `OP_STOP` | J-Type / R-Type | Halts the LVM virtual execution core immediately. |
| 1U | `OP_SYS` | J-Type / R-Type | Performs an execution environment system call. |
| 2U | `OP_LDS` | R-Type | Load signed data item from memory into a target register. |
| 3U | `OP_LDU` | R-Type | Load unsigned data item from memory into a target register. |
| 4U | `OP_LDF` | R-Type | Load floating-point numeric data item from memory. |
| 5U | `OP_STS` | R-Type | Store signed register value back out into memory. |
| 6U | `OP_STU` | R-Type | Store unsigned register value back out into memory. |
| 7U | `OP_STF` | R-Type | Store floating-point register value out into memory. |
| 8U | `OP_CPY` | R-Type | Copies data directly between two registers. |
| 9U | `OP_LIS8` | I-Type | Load immediate signed 8-bit literal constant. |
| 10U | `OP_LIS16` | I-Type | Load immediate signed 16-bit literal constant. |
| 11U | `OP_LIU8` | I-Type | Load immediate unsigned 8-bit literal constant. |
| 12U | `OP_LIU16` | I-Type | Load immediate unsigned 16-bit literal constant. |
| 13U | `OP_CRV` | R-Type | Clear register value: sets all bits of the target register to 0. |

### 5.2 Group 1 (16 – 31): Arithmetic Operations

| Opcode Value | Mnemonic | Format Type | Description |
| --- | --- | --- | --- |
| 16U | `OP_ADDS` | R-Type | Add signed operands. |
| 17U | `OP_SUBS` | R-Type | Subtract signed operands. |
| 18U | `OP_MULS` | R-Type | Multiply signed integers. |
| 19U | `OP_DIVS` | R-Type | Divide signed integers. |
| 20U | `OP_REMS` | R-Type | Calculate remainder (modulo) for signed integers. |
| 21U | `OP_ADDU` | R-Type | Add unsigned integers. |
| 22U | `OP_SUBU` | R-Type | Subtract unsigned integers. |
| 23U | `OP_MULU` | R-Type | Multiply unsigned integers. |
| 24U | `OP_DIVU` | R-Type | Divide unsigned integers. |
| 25U | `OP_REMU` | R-Type | Calculate remainder (modulo) for unsigned integers. |
| 26U | `OP_ADDF` | R-Type | Add floating-point values. |
| 27U | `OP_SUBF` | R-Type | Subtract floating-point values. |
| 28U | `OP_MULF` | R-Type | Multiply floating-point values. |
| 29U | `OP_DIVF` | R-Type | Divide floating-point values. |
| 30U | `OP_REMF` | R-Type | Calculate floating-point remainder. |

### 5.3 Group 2 (32 – 47): Bitwise Operations

| Opcode Value | Mnemonic | Format Type | Description |
| --- | --- | --- | --- |
| 32U | `OP_ASL` | R-Type | Arithmetic shift left. |
| 33U | `OP_ASR` | R-Type | Arithmetic shift right (preserves sign bit). |
| 34U | `OP_SL` | R-Type | Logical shift left. |
| 35U | `OP_SR` | R-Type | Logical shift right. |
| 36U | `OP_AND` | R-Type | Bitwise logical AND. |
| 37U | `OP_OR` | R-Type | Bitwise logical OR. |
| 38U | `OP_NOT` | R-Type | Bitwise logical NOT (unary inversion). |
| 39U | `OP_XOR` | R-Type | Bitwise logical exclusive OR. |

### 5.4 Groups 3 & 4 (48 – 79): Comparison & Control Flow Operations

| Opcode Value | Mnemonic | Format Type | Description |
| --- | --- | --- | --- |
| 48U | `OP_ES` | R-Type | Check equal (signed). Sets `REG_FL` matching state. |
| 49U | `OP_GS` | R-Type | Greater-than comparison (signed). |
| 50U | `OP_LS` | R-Type | Less-than comparison (signed). |
| 51U | `OP_EU` | R-Type | Check equal (unsigned). |
| 52U | `OP_GU` | R-Type | Greater-than comparison (unsigned). |
| 53U | `OP_LU` | R-Type | Less-than comparison (unsigned). |
| 54U | `OP_EF` | R-Type | Check equal (float). Handles floating-point rules. |
| 55U | `OP_GF` | R-Type | Greater-than comparison (float). |
| 56U | `OP_LF` | R-Type | Less-than comparison (float). |
| 57U | `OP_CF` | R-Type | Classify float (e.g., handles NaN, Inf properties). |
| 58U | `OP_BEC` | I-Type / J-Type | Branch on equal conditions via flags verification. |
| 59U | `OP_BUC` | I-Type / J-Type | Branch on unequal conditions via flags verification. |
| 60U | `OP_BE` | I-Type / J-Type | Branch if equal. |
| 61U | `OP_BU` | I-Type / J-Type | Branch if unequal. |
| 62U | `OP_BG` | I-Type / J-Type | Branch if greater. |
| 63U | `OP_BL` | I-Type / J-Type | Branch if less. |
| 64U | `OP_BGE` | I-Type / J-Type | Branch if greater or equal. |
| 65U | `OP_BLE` | I-Type / J-Type | Branch if less or equal. |
| 66U | `OP_BNAN` | I-Type / J-Type | Branch if float condition registers a NaN flag. |
| 67U | `OP_BINF` | I-Type / J-Type | Branch if float condition registers an infinity flag. |
| 68U | `OP_BERR` | I-Type / J-Type | Branch if an execution/system error flag is tripped. |
| 69U | `OP_JD` | J-Type | Jump direct to immediate address. Updates `REG_PC`. |
| 70U | `OP_JI` | J-Type / I-Type | Jump indirect via data storage reference pointers. |
| 71U | `OP_JR` | J-Type | Jump relative via target immediate value offset. |
| 72U | `OP_JIR` | R-Type | Jump indirect register to address kept in variable. |
| 73U | `OP_JRR` | R-Type | Jump relative register using displacement value. |
| 74U | `OP_PUSH` | R-Type / I-Type | Push single value onto the stack. |
| 75U | `OP_POP` | R-Type | Pop single value off the stack into a target register. |
| 76U | `OP_PUSHM` | R-Type | Push multiple registers onto the stack sequentially. |
| 77U | `OP_POPM` | R-Type | Pop multiple registers off the stack sequentially. |
| 78U | `OP_CALL` | J-Type / R-Type | Invokes a subroutine, pushing the old `REG_PC` to the stack frame. |
| 79U | `OP_RET` | None | Returns from subroutine, popping the saved return address. |

## 6. Flags & Classification Integration

The status evaluation functions (`OP_ES` through `OP_CF`) explicitly change the values of `REG_FL` (0U). The branch instruction pipeline relies on these bit-masks to selectively change code pathways. Floating-point handling using `OP_CF` provides isolated tracking states for anomalies, ensuring IEEE-like processing inside an explicit virtual environment.

### 6.1 Flags Register Layout

`REG_FL` (register 0) is a 64-bit register. Its bits are assigned as follows:

| Bit | Mask | Description |
| --- | --- | --- |
| 0 | `0x0000000000000001` | Equal flag |
| 1 | `0x0000000000000002` | Greater flag |
| 2 | `0x0000000000000004` | Less flag |
| 3 | `0x0000000000000008` | Overflow flag (signed arithmetic) |
| 4 | `0x0000000000000010` | Carry flag (unsigned arithmetic overflow) |
| 5 | `0x0000000000000020` | Inexact / Rounded (floating-point math) |
| 6 | `0x0000000000000040` | Underflow (floating-point math) |
| 7 | `0x0000000000000080` | Overflow (floating-point math) |
| 8 | `0x0000000000000100` | NaN |
| 9 | `0x0000000000000200` | Infinity |
| 10 | `0x0000000000000400` | Divide by 0 |
| 11 | `0x0000000000000800` | Invalid operation |
| 12 | `0x0000000000001000` | LVM internal error |
| 13 – 63 | `0xFFFFFFFFFFFFE000` | Reserved |