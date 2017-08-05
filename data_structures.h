#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_
#include <string.h>

#define MAX_DATA_ARR_SIZE 10
#define MAX_ROWS 10
#define MAX_COLS 10
#define NUM_OF_OPERAND_TYPES 4

enum data_type {NONE, DATA, CODE};
enum boolean {FALSE, TRUE};
enum guidance {EXTERN, ENTRY, NUM, STRING, MAT}; 

extern int size_opcode_table;

/* struct for holding an opcode and it's binary value */
typedef struct opcodes {
	char* opcode;
	char binary_val[4];
} opcodes;

typedef struct mem_words_per_operand_type {
	char operand_type[2];
	int num_of_mem_words;
} mem_words_per_operand_type;

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
	char bits[11]; /* there are 11 places so '\0' can be added */
	struct memory_word *next;
} memory_word;

typedef struct sentence {
	int is_action; /* 1 if it's an action sentence, 0 if it's a guidance sentence */
	int is_store_command; /* 1 if it's .data/.string./.mat, 0 if it's extern or entry */
	int guidance_command; /* enum: EXTERN / ENTRY / NUM (=DATA)  / STRING / MAT */
	int is_symbol; 
	char* symbol;
	char* opcode[5];
	int num_of_operands;
	char source_operand_type[3]; /* 3 places so '\0' can be added */
	char dest_operand_type[3];   /* 3 places so '\0' can be added */	
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

extern opcodes opcodes_table[];
extern mem_words_per_operand_type operands_vs_num_of_words_to_use[];
extern registers registers_table[];
 

/*
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

 in the table: how many memory words the operand type requires 
mem_words_per_operand_type operands_vs_num_of_words_to_use[] = {
	{"00", 1},
	{"01", 1},
	{"10", 2},
	{"11", 1}
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
 
*/
#endif

