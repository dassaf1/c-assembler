all: my_assembler
my_assembler: main.c assembler.c first_pass.c second_pass.c assembler.h number_conversions.c number_conversions.h data_structures.c data_structures.h line_parser.c line_parser.h
	gcc -g -Wall -ansi -pedantic main.c assembler.c first_pass.c second_pass.c number_conversions.c data_structures.c line_parser.c -o my_assembler -lm
	clean: rm *~
