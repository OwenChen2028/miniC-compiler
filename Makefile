compiler:
	lex miniC_lexer.l
	yacc -d miniC_parser.y
	g++ -g -Wno-write-strings lex.yy.c y.tab.c ast.c -o parser.out

clean:
	rm -f lex.yy.c y.tab.c y.tab.h parser.out
