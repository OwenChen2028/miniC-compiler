compiler:
	lex lexer.l
	yacc -d parser.y
	g++ -g -Wno-write-strings lex.yy.c y.tab.c ast.c analysis.c -o parser.out

clean:
	rm -f lex.yy.c y.tab.c y.tab.h parser.out
