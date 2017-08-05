#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DATA_ARR_SIZE 10
#define MAX_ROWS 10
#define MAX_COLS 10

enum data_type {NONE, DATA, CODE};
enum boolean {FALSE, TRUE};
enum guidance {EXTERN, ENTRY, NUM, STRING, MAT}; 

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

typedef struct symbol_line {
	char symbol[10];
	int address;
	int is_extern;
	int symbol_type; /* from enum: NONE, DATA, CODE */
	struct symbol_line *next;
} symbol_line;

/* struct for holding the IC \ DC (code memory word address \ data memory word address), an array of 10 bits (in chars), and a pointer to the next struct of the same type */
typedef struct memory_word { 
	int address;
	char bits[10];
	struct memory_word *next;
} memory_word;

typedef struct sentence {
	int is_action; /* 1 if it's an action sentence, 0 if it's a guidance sentence */
	int is_store_command; /* 1 if it's .data/.string./.mat, 0 if it's extern or entry */
	int guidance_command; /* enum: EXTERN / ENTRY / NUM (=DATA)  / STRING / MAT */
	int is_symbol; 
	char* symbol;
	char* opcode;
	int num_of_operands;
	char* source_operand_type;
	char* dest_operand_type;
	char* operand_1; /* for variables, registers, matrixes */ 
	char* operand_2; /* for variables, registers, matrixes */
	int immediate_operand; /* when we have "#" */
	char* matrix_row_operand; /* if we have M1[r1][r2] then r1 goes here */
	char* matrix_col_operand; /* r2 */
	char* string; /* if guidance_command  = .string, check for it's value in this field */
	int data_arr[MAX_DATA_ARR_SIZE];
	int data_arr_num_of_params; /* how many numbers to take from data_arr */
	int mat[MAX_ROWS * MAX_COLS];
	int mat_num_of_rows;
	int mat_num_of_cols; 
	struct sentence *next;
} sentence;

opcodes opcodes_table[] = {
	{ "mov", "0000"},
	{ "cmp", "0001"},
	{ "add", "0010"},
	{ "sub", "0011"},
	{ "not", "0100"},
	{ "clr", "0101"},
	{ "lea", "0110"},
	{ "inc", "0111"},
	{ "dec", "1000"},
	{ "jmp", "0101"},
	{ "bne", "1010"},
	{ "red", "1011"},
	{ "prn", "1100"},
	{ "jsr", "1101"},
	{ "rts", "1110"},
	{ "stop", "1111"}
};


int size_opcode_table = sizeof(opcodes_table)/sizeof(opcodes_table[0]);

registers registers_table[] = {
	{"r0", "000"},
	{"r1", "001"},
	{"r2", "010"},
	{"r3", "011"},
	{"r4", "100"},
	{"r5", "101"},
	{"r6", "110"},
	{"r7", "111"}
};
 
#endif

