#pragma once

#include <llvm-c/Core.h>
#include <unordered_map>

enum Register {
  ebx,
  ecx,
  edx,
  nullreg
};

extern std::unordered_map<LLVMValueRef, Register> reg_map;
