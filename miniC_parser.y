%{
#include <stdio.h>

extern int yylex();
extern int yylex_destroy();
extern int yywrap();

int yyerror(char *);

extern FILE *yyin;
%}
%union {
  
}

%token ID NUM
%token INT VOID
%token IF ELSE WHILE
%token RETURN
%token EQ GE LE
%token EXTERN
%token READ PRINT
%start start

%%
start : printDec readDec funcDec

printDec : EXTERN VOID PRINT '(' INT ')' ';'
readDec : EXTERN INT READ '(' ')' ';'
funcDec : INT ID '(' ')' '{' block '}'
        | INT ID '(' INT ID ')' '{' block '}'

block : stmt | stmt block
stmt : varDec | varAssign | printStmt | ifStmt | whileStmt | retStmt

varDec : INT ID ';'
varAssign : ID '=' intExpr ';'
          | ID '=' readCall ';'

readCall : READ '(' ')'
printStmt : PRINT '(' term ')' ';'

ifStmt : IF '(' boolExpr ')' ifBody
       | IF '(' boolExpr ')' ifBody ELSE ifBody
ifBody : stmt | '{' block '}'

whileStmt : WHILE '{' block '}'

retStmt : RETURN intExpr ';'
        | RETURN '(' intExpr ')' ';'

intExpr : term
        | term arithOp term
arithOp : '+' | '-' | '*' | '/'

boolExpr : term logicOp term
logicOp : '>' | '<' | EQ | GE | LE

term : NUM | ID
%%

int yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
      fprintf(stderr, "File open error");
      return 1;
    }
  }

  yyparse();
  return 0;
}
