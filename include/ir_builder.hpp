#pragma once

#include "ast.hpp"
#include <llvm-c/Core.h>

extern LLVMModuleRef module;

void build_ir(astNode *root);
