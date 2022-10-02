all:
	@$(CC) -O2 -g0 -s -o UBE UBE.c

clean:
	@rm -f UBE
