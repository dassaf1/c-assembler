#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "number_conversions.h"

#define instruction_source_oper_start_bit 4
#define instruction_dest_oper_start_bit 6
#define instruction_ARE_start_bit 8
#define absolute_ARE "00"
#define External_ARE "01"
#define Relocatable_ARE "10"
#define code_table_start_address 100

memory_word *code_head = NULL;
memory_word *code_tail = NULL;

void code_instruction(sentence * curr, int address);
void code_operand(int oper_type);
void code_immediate_operand(int oper, int address);
void code_direct_operand(char * oper);
void code_register(int oper_position);
void add_item_to_code_list(memory_word * item);

void add_item_to_code_list(memory_word * item) { /* Code list is a linked list of memory words */
    if (!code_head) { /* First element in linked list */
        code_head = code_tail = item;
    }
    else {
        code_tail->next = item;
        code_tail = item;
    }
}

memory_word * create_new_memory_word() {
    memory_word * new_memory_word = (memory_word*)malloc(sizeof(memory_word));
    if (!new_memory_word) {
		fprintf(stderr, "Error: Memory allocation failed");
		return NULL;
	}
	return new_memory_word;
}

void code_immediate_operand(int oper, int address) {
    char arr[8];
    memory_word * new_memory_word = create_new_memory_word();
    new_memory_word->address = address;
    convert_dec_to_x_bit_binary(oper, 8, arr);
    strcpy(new_memory_word->bits, arr);
    strcpy(((new_memory_word->bits)+instruction_ARE_start_bit), absolute_ARE);
    add_item_to_code_list(new_memory_word);
    printf("address: %d, bits: %s\n", new_memory_word->address, new_memory_word->bits);
}

void code_instruction(sentence * curr, int address) {
    int i;
    memory_word * new_memory_word = create_new_memory_word();
	new_memory_word->address = address;
	for(i = 0; i < sizeof(opcodes_table) / sizeof(struct opcodes); i++) {
        if (strcmp(opcodes_table[i].opcode, curr->opcode) == 0) {
            strcpy(new_memory_word->bits, opcodes_table[i].binary_val);
        }
    }
    strcpy(((new_memory_word->bits)+instruction_source_oper_start_bit), curr->source_operand_type);
    strcpy(((new_memory_word->bits)+instruction_dest_oper_start_bit), curr->dest_operand_type);
    strcpy(((new_memory_word->bits)+instruction_ARE_start_bit), absolute_ARE); /* Instruction is always type absolute */
    printf("address: %d, bits: %s\n", new_memory_word->address, new_memory_word->bits);
    add_item_to_code_list(new_memory_word);
}

int main() {
    sentence * curr = (sentence *)malloc(sizeof(sentence));
    strcpy(curr->opcode, "stop");
    strcpy(curr->source_operand_type, "99");
    strcpy(curr->dest_operand_type, "88");
    code_instruction(curr, code_table_start_address);
    code_immediate_operand(30, 1);
    return 0;
}

/*
void execute_second_pass(FILE *fd) {
    char line[80];
char *current_word;
int last_position;
int IC = 116;
int DC = 12;

    sentence *current_sentence;
    int is_error_found = 0;
    int address = 0;

	while(fgets(line,80,fd))
	{
		current_sentence = parse_sentence(line);
		line_number++;
		if(current_sentence->is_instruction == 0) {
		    if (strcmp(current_sentece->command_type, "entry") == 0) {

		    }
		}
	}

    if (is_error_found) {
        return;
    }

    create_object_file()
}

void create_object_file() {
    // write DC and IC size in strange 4
    // write code table in 4 strage
}
*/