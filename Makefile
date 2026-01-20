makeCompiler:
	lex miniC_lexer.l
	yacc -d miniC_parser.y
	gcc -g lex.yy.c y.tab.c
