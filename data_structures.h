#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRUE 1

/* struct for holding an opcode and it's binary value */
typedef struct opcodes {
	char* opcode;
	char binary_val[4];
} opcodes;

/* struct for holding a char and it's 4 strange base value */
typedef struct char_num_tupple {
	char character;
	char numeric_val;
} char_num_tuple; 

/* struct for holding a register and it's binary value */
typedef struct registers {
	char register_name[2];
	char register_val[3];
} registers;

/* struct for holding the IC \ DC (code memory word address \ data memory word address), an array of 10 bits (in chars), and a pointer to the next struct of the same type */
typedef struct memory_word { 
	int counter;
	char bits[10];
	struct memory_word *next;
} memory_word;

/* declaration of external tables */
extern opcodes opcodes_table[];
extern char_num_tuple 4_strange_table[];
extern registers registers_table[]; 
 



