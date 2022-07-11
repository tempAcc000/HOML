# Makefile


program = translator
objs = io.o scanner.o preprocess.o gen_smt2.o gen_smt2_bv.o gen_yices.o main.o


CC = gcc
CFLAGS = -g -Wall


$(program): $(objs)
	$(CC) -o $(program) $^


.PHONY: clean
clean:
	$(RM) $(program) $(objs)


io.o: io.c translator.h
scanner.o: scanner.c
preprocess.o: preprocess.c translator.h
gen_smt2.o: gen_smt2.c translator.h
gen_smt2_bv.o: gen_smt2_bv.c translator.h
gen_yices.o: gen_yices.c translator.h
main.o: main.c translator.h
