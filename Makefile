compiler:
	lex src/lexer.l
	yacc -d src/parser.y
	mkdir -p gen
	mv lex.yy.c y.tab.c y.tab.h gen/
	g++ -g src/compiler.c \
		-Igen gen/lex.yy.c gen/y.tab.c \
		-Iinclude src/ast.c src/analysis.c src/ir_builder.c src/optimizer.c src/backend.c \
		`llvm-config-18 --cxxflags --ldflags --system-libs --libs core support` \
		-o compiler.out

clean:
	rm -rf gen
	rm -f *.out
