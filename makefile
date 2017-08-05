all: my_assembler
my_assembler: main.c assembler.c assembler.h number_conversions.c number_conversions.h data_structures.h
	gcc -g -Wall -ansi -pedantic main.c assembler.c number_conversions.c -o my_assembler -lm
	clean: rm *~
