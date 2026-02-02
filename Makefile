compiler:
	lex miniC_lexer.l
	yacc -d miniC_parser.y
	gcc -g lex.yy.c y.tab.c -o parser

clean:
	rm -f lex.yy.c y.tab.c y.tab.h parser
