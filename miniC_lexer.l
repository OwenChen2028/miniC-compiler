%{
#include "y.tab.h"
extern int yyerror(char *);
%}

%%
-?[0-9] { return NUM; }
[a-zA-Z0-9_] { return ID; }
int { return INT; }
void { return VOID; }
if { return IF; }
else { return ELSE; }
while { return WHILE; }
return { return RETURN; }
[=][=] { return EQ; }
[>][=] { return GE; }
[<][=] { return LE; }
extern { return EXTERN; }
read { return READ; }
print { return PRINT; }
[ \t\r\n]
. { return yytext[0]; }
%%

int yywrap(void) {
  return 1;
}
