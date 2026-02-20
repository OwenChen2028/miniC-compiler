compiler:
	make -C frontend parser
	make -C optimizer optimizer

clean:
	make -C frontend clean
	make -C optimizer clean
