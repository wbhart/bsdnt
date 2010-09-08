check:
	gcc -O2 -g -pedantic -Wall nn.c t-nn.c -o t-nn
	./t-nn

clean:
	rm t-nn

