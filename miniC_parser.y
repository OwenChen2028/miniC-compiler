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
%token EQ NE GE LE
%token EXTERN
%token READ PRINT
%start start

%nonassoc TAG
%nonassoc ELSE

%%
start : printDec readDec funcDec

printDec : EXTERN VOID PRINT '(' INT ')' ';'
readDec : EXTERN INT READ '(' ')' ';'
funcDec : INT ID '(' ')' block
        | INT ID '(' INT ID ')' block

block : '{' decls stmts '}'

decls : /* empty */ | decls varDec
varDec : INT ID ';'

stmts : /* empty */ | stmts stmt
stmt : varAssign | printStmt | ifStmt | whileStmt | retStmt | block

varAssign : ID '=' intExpr ';'
          | ID '=' readCall ';'

readCall : READ '(' ')'
printStmt : PRINT '(' intExpr ')' ';'

ifStmt : IF '(' boolExpr ')' stmt %prec TAG
       | IF '(' boolExpr ')' stmt ELSE stmt

whileStmt : WHILE '(' boolExpr ')' stmt

retStmt : RETURN intExpr ';'
        | RETURN '(' intExpr ')' ';'

boolExpr : intExpr logicOp intExpr
logicOp : '>' | '<' | EQ | NE | GE | LE

intExpr : term
        | term arithOp term
arithOp : '+' | '-' | '*' | '/'

term : posTerm | '-' posTerm
posTerm : NUM | ID
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
