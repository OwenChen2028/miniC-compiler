#pragma once

#include "ast.h"
#include <llvm-c/Core.h>

extern LLVMModuleRef module;

void build_ir(astNode *root);
