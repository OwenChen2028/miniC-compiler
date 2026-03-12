#pragma once

#include <llvm-c/Core.h>
#include <unordered_map>

enum Register {
  ebx,
  ecx,
  edx,
  nullreg
};

void generate_code(LLVMModuleRef module);
