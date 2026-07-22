%{
#include "ast.hpp"
#include "parser.hpp"
#include <vector>
extern int yyerror(char *);
%}

%%
int { return INT; }
void { return VOID; }
if { return IF; }
else { return ELSE; }
while { return WHILE; }
return { return RETURN; }
extern { return EXTERN; }
read { return READ; }
print { return PRINT; }
"==" { return EQ; }
"!=" { return NE; }
">=" { return GE; }
"<=" { return LE; }
[0-9]+ { yylval.ival = atoi(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return NAME; }
[ \t\r\n]+ { /* skip */ }
. { return yytext[0]; }
%%

int yywrap(void) {
  return 1;
}
