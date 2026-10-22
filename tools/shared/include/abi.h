#pragma once

#ifndef ABI_VERSION
#define ABI_VERSION 1       // fallback to default ABI v1.0
#define ISA_VERSION 1       // ABI v1.0 uses ISA v1.0
#endif

#include "isa.h"

#define ARGUMENT_REGISTERS 16U  
#define ARGUMENT_REGISTER_START 8U
#define ARGUMENT_REGISTER_END 23U

#define VOLATILE_REGISTERS 124U
#define VOLATILE_REGISTER_START 24U
#define VOLATILE_REGISTER_END 131U

#define NON_VOLATILE_REGISTERS 124U
#define NON_VOLATILE_REGISTER_START 132U
#define NON_VOLATILE_REGISTER_END 255U

