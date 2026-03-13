#pragma once

#include <cstdio>
#include <llvm-c/Core.h>

enum Register { eax, ebx, ecx, edx, nullreg };

extern FILE *out_fp;

void generate_code(LLVMModuleRef module);
