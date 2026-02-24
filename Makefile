compiler:
	make -C frontend parser
	make -C backend optimizer

clean:
	make -C frontend clean
	make -C backend clean
