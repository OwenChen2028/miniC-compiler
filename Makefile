compiler:
	mkdir -p gen
	lex -o gen/lexer.cpp src/lexer.l
	yacc -d -o gen/parser.cpp src/parser.y
	g++ -g src/compiler.cpp \
		-Igen gen/lexer.cpp gen/parser.cpp \
		-Iinclude src/ast.cpp src/analysis.cpp src/ir_builder.cpp src/optimizer.cpp src/backend.cpp \
		`llvm-config-18 --cxxflags --ldflags --system-libs --libs core support` \
		-o compiler.out

clean:
	rm -rf gen
	rm -f *.out
	rm -f *.s
