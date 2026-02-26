#include "ast.h"
#include "y.tab.h"
#include "analysis.h"
#include "ir_builder.h"
#include <cstdio>

extern astNode *root;

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
      printNode(root);
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
  }
}
