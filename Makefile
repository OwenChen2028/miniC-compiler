compiler:
	lex lexer.l
	yacc -d parser.y
	g++ compiler.c ast.c lex.yy.c y.tab.c analysis.c ir_builder.c -o compiler.out

clean:
	rm -f lex.yy.c y.tab.c y.tab.h
	rm -f compiler.out
