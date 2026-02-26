compiler:
	lex lexer.l
	yacc -d parser.y
	g++ -g compiler.c ast.c lex.yy.c y.tab.c analysis.c ir_builder.c optimizer.c -o compiler.out \
		`llvm-config --cxxflags --ldflags --system-libs --libs core support`

clean:
	rm -f lex.yy.c y.tab.c y.tab.h
	rm -f *.out
	rm -r *.ll

