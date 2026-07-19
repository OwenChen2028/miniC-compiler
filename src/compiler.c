#include "analysis.h"
#include "ast.h"
#include "backend.h"
#include "ir_builder.h"
#include "optimizer.h"
#include "y.tab.h"
#include <cstdio>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Support.h>

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
  }

  int result = yyparse();
  if (result == 0) {
    if (!root) {
      fprintf(stderr, "Error: AST root is NULL.\n");
      return 1;
    }

    result = semantic_analysis(root);
    switch (result) {
    case 0:
      // printNode(root);
      build_ir(root);
      if (!module) {
        fprintf(stderr, "Error: LLVM Module is NULL.\n");
        return 1;
      }
      // LLVMDumpModule(module);
      optimize_ir(module);
      // LLVMDumpModule(module);
      if (argc >= 3) {
        out_fp = fopen(argv[2], "w");
        if (out_fp == NULL) {
          fprintf(stderr, "Error: failed to open output.\n");
          result = 1;
          break;
        }
      } else {
        out_fp = stdout;
      }
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

  return result != 0;
}
