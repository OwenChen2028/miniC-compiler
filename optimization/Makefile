optimizer:
	g++ -g -I /usr/include/llvm-c-17/ -c optimizer.c
	g++ optimizer.o `llvm-config-17 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-17/ -o optimizer.out

clean:
	rm -f optimizer.out
	rm -f *.o
