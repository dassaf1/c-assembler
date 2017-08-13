#include "data_structures.h"

opcodes opcodes_table[] = { 
	{ "mov", "0000",2,{0,1,2,3,-1},{1,2,3,-1}},
	{ "cmp", "0001",2,{0,1,2,3,-1},{0,1,2,3,-1}},
	{ "add", "0010",2,{0,1,2,3,-1},{1,2,3,-1}},
	{ "sub", "0011",2,{0,1,2,3,-1},{1,2,3,-1}},
	{ "not", "0100",1,{-1},{1,2,3,-1}},
	{ "clr", "0101",1,{-1},{1,2,3,-1}},
	{ "lea", "0110",1,{1,2,-1},{1,2,3,-1}},
	{ "inc", "0111",1,{-1},{1,2,3,-1}},
	{ "dec", "1000",1,{-1},{1,2,3,-1}},
	{ "jmp", "0101",1,{-1},{1,2,3,-1}},
	{ "bne", "1010",1,{-1},{1,2,3,-1}},
	{ "red", "1011",1,{-1},{1,2,3,-1}},
	{ "prn", "1100",1,{-1},{0,1,2,3,-1}},
	{ "jsr", "1101",1,{-1},{1,2,3,-1}},
	{ "rts", "1110",1,{-1},{-1}},
	{ "stop", "1111",1,{-1},{-1}}
};

int size_opcode_table = sizeof(opcodes_table)/sizeof(opcodes_table[0]);

/* in the table: how many memory words the operand type requires */
mem_words_per_operand_type operands_vs_num_of_words_to_use[] = {
	{"00", 1},
	{"01", 1},
	{"10", 2},
	{"11", 1}
};

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

char *saved_languages_words[NUM_OF_SAVED_WORDS] = {"extern", "entry", "data", "mat", "string", "mov", "cmp", "add", "sub", "not", "clr", "lea", 
					"inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};




