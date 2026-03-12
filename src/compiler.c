#include "analysis.h"
#include "ast.h"
#include "backend.h"
#include "ir_builder.h"
#include "optimizer.h"
#include "y.tab.h"
#include <cstdio>
#include <llvm-c/Core.h>

extern astNode *root;
extern LLVMModuleRef module;
extern FILE *out_fp;

extern FILE *yyin;
extern void yylex_destroy();

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
      fprintf(stderr, "Error: failed to open input.\n");
      return 1;
    }

    if (argc >= 3) {
      out_fp = fopen(argv[2], "w");
      if (out_fp == NULL) {
        fprintf(stderr, "Error: failed to open output.\n");
        return 1;
      }
    }
  }

  if (yyparse() == 0) {
    if (!root) {
      fprintf(stderr, "Error: AST root is NULL.\n");
      return 1;
    }

    switch (semantic_analysis(root)) {
    case 0:
      //printNode(root);
      build_ir(root);
      if (!module) {
        fprintf(stderr, "Error: LLVM Module is NULL.\n");
        return 1;
      }
      //LLVMDumpModule(module);
      optimize_ir(module);
      //LLVMDumpModule(module);
      generate_code(module);
      break;
    case 1:
      printf("Variable used before declaration.\n");
      break;
    case 2:
      printf("Variable declared twice in one scope.\n");
      break;
    }

    yylex_destroy();
    freeNode(root);
    LLVMDisposeModule(module);
    LLVMShutdown();
  }
}
