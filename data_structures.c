#include "data_structures.h"

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






