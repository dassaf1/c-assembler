#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <stdio.h>
#include "data_structures.h"
#define CODE_TABLE_START_ADDRESS 100
#define OPCODES_TABLE_LENGTH 16

int IC;
int DC;
symbol_line *symbol_tail;
symbol_line *symbol_head;
memory_word *data_tail;
memory_word *data_head;
sentence *sentence_head;
sentence *sentence_tail;

void free_data(memory_word *data_head);
void free_symbol(symbol_line *symbol_head);
void free_sentence(sentence *sentence_head);
int execute_first_pass(FILE *);

/* void run_second_pass(char *);*/
#endif
