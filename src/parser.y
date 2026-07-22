%{
#include "ast.hpp"
#include <cstdio>
#include <vector>

astNode *root;

extern int yylex();
extern int yylex_destroy();
extern int yywrap();

int yyerror(const char *);
%}

%union {
  int ival;
  char *sval;
  astNode *node;
  std::vector<astNode *> *nvec; 
}

%token <ival> NUM
%token <sval> NAME
%token INT VOID
%token IF ELSE WHILE
%token RETURN
%token EQ NE GE LE
%token EXTERN
%token READ PRINT

%start program

%nonassoc TAG
%nonassoc ELSE

%type <node> printDec readDec funcDec
%type <node> block varDec stmt varAssign
%type <node> readCall printStmt
%type <node> ifStmt whileStmt retStmt
%type <node> relExpr intExpr term posTerm
%type <nvec> blockBody decls stmts

%%
program : printDec readDec funcDec { root = createProg($1, $2, $3); }

printDec : EXTERN VOID PRINT '(' INT ')' ';' { $$ = createExtern("print"); }

readDec : EXTERN INT READ '(' ')' ';' { $$ = createExtern("read"); }

funcDec : INT NAME '(' ')' block { $$ = createFunc($2, NULL, $5); }
        | INT NAME '(' INT NAME ')' block { $$ = createFunc($2, createVar($5), $7); }

block : '{' blockBody '}' { $$ = createBlock($2); }

blockBody : decls stmts { $$ = $1; $$->insert($$->end(), $2->begin(), $2->end()); delete $2; }

decls : /* empty */ { $$ = new std::vector<astNode *>(); }
      | decls varDec { $$ = $1; $$->push_back($2); }

varDec : INT NAME ';' { $$ = createDecl($2); }

stmts : /* empty */ { $$ = new std::vector<astNode *>(); }
      | stmts stmt { $$ = $1; $$->push_back($2); }

stmt : varAssign { $$ = $1; }
     | printStmt { $$ = $1; }
     | ifStmt { $$ = $1; }
     | whileStmt { $$ = $1; }
     | retStmt { $$ = $1; }
     | block { $$ = $1; }

varAssign : NAME '=' intExpr ';' { $$ = createAsgn(createVar($1), $3); }
          | NAME '=' readCall ';' { $$ = createAsgn(createVar($1), $3); }

readCall : READ '(' ')' { $$ = createCall("read"); }

printStmt : PRINT '(' intExpr ')' ';' { $$ = createCall("print", $3); }

ifStmt : IF '(' relExpr ')' stmt %prec TAG { $$ = createIf($3, $5); }
       | IF '(' relExpr ')' stmt ELSE stmt { $$ = createIf($3, $5, $7); }

whileStmt : WHILE '(' relExpr ')' stmt { $$ = createWhile($3, $5); }

retStmt : RETURN intExpr ';' { $$ = createRet($2); }
        | RETURN '(' intExpr ')' ';' { $$ = createRet($3); }

relExpr : intExpr '<' intExpr { $$ = createRExpr($1, $3, lt); }
        | intExpr '>' intExpr { $$ = createRExpr($1, $3, gt); }
        | intExpr EQ intExpr { $$ = createRExpr($1, $3, eq); }
        | intExpr NE intExpr { $$ = createRExpr($1, $3, neq); }
        | intExpr GE intExpr { $$ = createRExpr($1, $3, ge); }
        | intExpr LE intExpr { $$ = createRExpr($1, $3, le); }

intExpr : term { $$ = $1; }
        | term '+' term { $$ = createBExpr($1, $3, add); }
        | term '-' term { $$ = createBExpr($1, $3, sub); }
        | term '*' term { $$ = createBExpr($1, $3, mul); }

term : posTerm { $$ = $1; }
     | '-' posTerm { $$ = createUExpr((astNode *)$2, uminus); }

posTerm : NUM { $$ = createCnst($1); }
        | NAME { $$ = createVar($1); }
%%

int yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
  return 0;
}
