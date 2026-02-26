#pragma once

#include "ast.h"
#include <llvm-c/Core.h>

extern LLVMModuleRef llvm_module;

void build_ir(astNode *root);
