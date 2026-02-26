#include "analysis.h"
#include "ast.h"
#include "ir_builder.h"
#include "y.tab.h"
#include <cstdio>
#include <llvm-c/Core.h>

extern astNode *root;
extern LLVMModuleRef llvm_module;

extern FILE *yyin;
extern void yylex_destroy();

int main(int argc, char *argv[]) {
  if (argc == 2) {
    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
      fprintf(stderr, "Error opening file.\n");
      return 1;
    }
  }

  if (yyparse() == 0) {
    if (root == NULL) {
      fprintf(stderr, "AST root is null.\n");
      return 1;
    }

    switch (semantic_analysis(root)) {
    case 0:
      build_ir(root);
      // if module valid, run optimizer
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
    //    LLVMDisposeModule(llvm_module);
    LLVMShutdown();
  }
}
