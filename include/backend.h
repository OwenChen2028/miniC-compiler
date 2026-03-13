#pragma once

#include <llvm-c/Core.h>
#include <cstdio>

enum Register {
  eax,
  ebx,
  ecx,
  edx,
  nullreg
};

extern FILE *out_fp;

void generate_code(LLVMModuleRef module);
