#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "number_conversions.h"
#include "line_parser.h"
#include "assembler.h"


#define LINE_LENGTH 81

char line[LINE_LENGTH]; 
char *current_symbol;
int last_position;
int syntax_errors = FALSE;
int line_number;

int IC = CODE_TABLE_START_ADDRESS;
int DC = 0;
symbol_line *symbol_tail = NULL;
symbol_line *symbol_head = NULL;
memory_word *data_tail = NULL;
memory_word *data_head = NULL;
sentence *sentence_head = NULL;
sentence *sentence_tail = NULL;

/* symbol_exists - 
 receives: the head of the symbols table, the current symbol. The function searches within the list if the symbol already exists.
 returns: 1 if already exists, 0 if not. */
int symbol_exists(char *current_symbol) 
{
	symbol_line *temp = symbol_head; 

	while (temp)
	{
		if (strcmp(temp->symbol, current_symbol) == 0)
		return TRUE; 

		else
			temp = temp->next;

	}
	
	return FALSE;
}

/* add_to_symbol_table -
 receives: the last line in the symbols table, the current symbol to be added, the address where it's going to be allocated (DC or 0 if it's extern),
 1 if the symbol is extern / 0 if not, the symbol type (from enum: NONE, DATA, CODE). The function allocates a new node
 of type symbol_line to the symbols table. */  
void add_to_symbol_table(char* current_symbol, int address, int is_extern, int symbol_type)
{
	symbol_line* new_symbol = (symbol_line*)malloc(sizeof(symbol_line));

	if (!new_symbol)
		{
			fprintf(stderr, "Memory allocation for new symbol failed!");
			exit(1);
		}
	
	strcpy(new_symbol->symbol,current_symbol); /* NTS: make sure that the current symbol gets '\0'. strcpy 
							copies the '\0' as well) */

	new_symbol->address = address;
	new_symbol->is_extern = is_extern;
	new_symbol->symbol_type = symbol_type; 

	if(!symbol_head) {
		symbol_head = new_symbol;
		symbol_tail = symbol_head;
		}
	else {
		symbol_tail->next = new_symbol;
		symbol_tail = symbol_tail->next;
	}

	symbol_tail->next = NULL;

	return;

}

/* add_to_memory_table - 
 receives: the last line in the memory table, a pointer to the bits to add, and the number of DC / IC of the word.
 This function allocates a new memory_word and add to the memory table that is the DATA MEMORY table, or to the CODE MEMORY table - it depends which table's
 tail is passed. 
 returns: the last added memory word. */
memory_word* add_to_memory_table(memory_word *tail, char *bits, int counter)
{
	memory_word* new_memory_word = (memory_word*)malloc(sizeof(memory_word));

	if (!new_memory_word)
		{
			fprintf(stderr, "Memory allocation for new memory word failed!");
			exit(1);
		}
	
	strcpy(new_memory_word->bits,bits); /* NTS: make sure that the bits array source gets '\0'. strcpy 
							copies the '\0' as well) */
	new_memory_word->address = counter;

	if(!tail)
		tail = new_memory_word;
	else {
		tail->next = new_memory_word;
		tail = tail->next;
	}

	tail->next = NULL;

	return tail;
}


/* is_existing_opcode - 
 receives: the current word to parse. The function checks if the word passed exists in the opcodes_table[] initialized in data_structures.c.
 returns: the position of the opcode in the list + 1 (for example, if the word passed is the first word in opcodes_table[], 
 the function will return 1, if it's the second it will return 2 and etc...). 0 if the word is not in the opcodes table. */
int is_existing_opcode(char *current_word)
{
	int i;
	for (i = 0; i < OPCODES_TABLE_LENGTH; i++)
	{
		if (strcmp(opcodes_table[0].opcode,current_word) == 0)
			return TRUE;
	}
	
	return FALSE;
}

/* add_string_to_data_table - 
   receives the sentence after it was parsed and converts each char of the string to it's binary representation for it's ascii value.
   returns the number of memory words added to the data table. */
int add_string_to_data_table(sentence *curr)
{
	int i;
	char *binary_char;
	memory_word* new_memory_word; 
	int added_mem_words = 0;

	for (i=0; i < strlen(curr->string); i++) {
	
		new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		if (!new_memory_word)
		{
			fprintf(stderr, "Memory allocation for new memory word failed!");
			exit(1);
		}
		
		convert_ascii_value_to_10_bit_binary(curr->string[i], new_memory_word->bits);
		
	
		if (data_tail)
		{
			data_tail->next = new_memory_word;
			data_tail = data_tail->next;
		}

		else {
			data_head = new_memory_word;
			data_tail = data_head;
			}
		
		added_mem_words++; 
	}
		/* adding '\0' to data table at the end of the string */
		new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		strcpy(new_memory_word->bits, "0000000000");
		
		added_mem_words++; 
		data_tail->next = new_memory_word;
		data_tail = data_tail->next;

		data_tail->next = NULL;
	
		return added_mem_words;
}


/* add_num_to_data_table - 
   receives the sentence after it was parsed and converts each number of the data_arr of the sentence into it's binary value.
   returns the number of memory words added to the data table. */
int add_num_to_data_table(sentence *curr)
{	
	int i;
	char converted_to_bits[11];
	memory_word* new_memory_word;
	int added_mem_words = 0;
	
	for(i=0; i < curr->data_arr_num_of_params ; i++) {
	
	new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		if (!new_memory_word)
		{
			fprintf(stderr, "Memory allocation for new memory word failed!");
			exit(1);
		}
	
	convert_dec_to_x_bit_binary(curr->data_arr[i],10,converted_to_bits);
    strcpy(new_memory_word->bits,converted_to_bits);
	if (data_tail) 
	{
		data_tail->next = new_memory_word;
		data_tail = data_tail->next;
	}
	else {
		data_head = new_memory_word;
		data_tail = data_head;
		}
		
		added_mem_words++; 	 
	}

	data_tail->next = NULL;
		
	return added_mem_words;
}


/* add_num_to_data_table - 
   receives the sentence after it was parsed and converts each number of the data_arr of the sentence into it's binary value.
   returns the number of memory words added to the data table. */

int add_matrix_to_data_table(sentence *curr)
{
	int i;
	memory_word* new_memory_word;
	int added_mem_words = 0;
	
	for(i=0; i < (curr->mat_num_of_rows)*(curr->mat_num_of_cols); i++) {
	
	new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		if (!new_memory_word)
		{
			fprintf(stderr, "Memory allocation for new memory word failed!");
			exit(1);
		}
	
	convert_dec_to_x_bit_binary(curr->mat[i],10,new_memory_word->bits);
	if (data_tail)
	{
		data_tail->next = new_memory_word;
		data_tail = data_tail->next;
	}
	else {
		data_head = new_memory_word;
		data_tail = data_head;
		}
		
		added_mem_words++; 	 
	}

	data_tail->next = NULL;
		
	return added_mem_words;
}


/* add_to_data_table -
   receives: the current sentence (line after it was parsed) and adds to the data tables the data according it's type.
   Adds to DC the number of entries commited */
void add_to_data_table(sentence* curr)
{
	int num_of_entries = 0; 
	
	switch(curr->guidance_command)
	{
		case(STRING):
			num_of_entries = add_string_to_data_table(curr);
			break;
		case(NUM):
			num_of_entries = add_num_to_data_table(curr);
			break;
		case(MAT):
			num_of_entries = add_matrix_to_data_table(curr);
			break;
		default:
			return;
	}

	DC+=num_of_entries;
	return;
		
}

/* add_to_IC_by_operand_type -
   receives: an operand to search for at the operands_vs_num_of_words_to_use table (initialized in data_structures.h).
   The function adds to IC the number of memory words that the type of operand uses. */
void add_to_IC_by_operand_type(char *operand)
{
	int i;
	for (i = 0; i < NUM_OF_OPERAND_TYPES; i++)
	{
		if(strcmp(operand,operands_vs_num_of_words_to_use[i].operand_type)==0) {
			IC += operands_vs_num_of_words_to_use[i].num_of_mem_words;
			return;
		}	
	}
	
	return;
}

/* increase_IC_according_sentence - 
   receives: the current sentence (the line after it was parsed) and checks for source and destination operands.
   The function increases IC according the number of memory words each type of operand requires. */
void increase_IC_according_sentence(sentence *curr)
{	
	IC++;  /* for the action sentence itself */	
	if(strcmp(curr->source_operand_type,curr->dest_operand_type)==0 && strcmp(curr->source_operand_type,REGISTER_OPERAND_TYPE)==0) {
	/* if both operands are "straight forwards registers" type, they consume a common single word */
		IC++;
		return;
	}
	else
	add_to_IC_by_operand_type(curr->source_operand_type);
	add_to_IC_by_operand_type(curr->dest_operand_type);
	return;
}

/* increase_DC_symbol_address_by_IC_offset -
   The function goes over the symbols table and adds to each entry of type DATA the IC offset. */
void increase_DC_symbol_address_by_IC_offset()
{
	symbol_line *temp;

	if(!symbol_head)
		return;
	
	temp = symbol_head;
	
	while(temp) {
		if (temp->symbol_type == DATA)
			temp->address += IC;
	
		temp = temp->next;
	}
	
	return;
}

/* free_data - 
   receives: the head of the data list and free memory. */
void free_data(memory_word *data_head)
{
	memory_word *temp;

	while (data_head)
	{
		temp = data_head->next;
		free(data_head);
		data_head = temp;
	}

	return;

}

/* free_symbol -
   receives: the head of the symbol list and free memory. */
void free_symbol(symbol_line *symbol_head)
{
	symbol_line *temp;

	while (symbol_head)
	{
		temp = symbol_head->next;
		free(symbol_head);
		symbol_head = temp;
	}

	return;

}

/* free_sentence -
   receives: the head of the symbol list and free memory. */
void free_sentence(sentence *sentence_head)
{
	sentence *temp;

	while (sentence_head)
	{
		temp = sentence_head->next;
		free(sentence_head);
		sentence_head = temp;
	}

	return;

}



int execute_first_pass(FILE *fd)
{ 
	sentence *current_sentence;	
	line_number = 0;	
	
		
	while(fgets(line,LINE_LENGTH,fd))
	{	
		/* ADD TO CHANGES */
		line_number++;		
		if (line[0] == ';')
			continue;

		current_sentence = parse_sentence(line, line_number, &syntax_errors);

		/* ADD TO CHANGES */
		if (current_sentence == NULL) /* if line is empty parse_sentence returns null */
			continue;

		if (current_sentence->is_store_command == TRUE) {
			if (current_sentence->is_symbol) {
				if (!symbol_exists(current_sentence->symbol)) {
					add_to_symbol_table(current_sentence->symbol, DC, 0, DATA);
				}
				else
					fprintf(stderr, "Error in line %d: symbol %s already exists in symbols table\n", line_number, current_sentence->symbol);
			}
			/* adding into data table. DC is increased: */
			add_to_data_table(current_sentence);
		}

		else { /* when is_store_command = 0 */
			if (current_sentence->guidance_command == EXTERN) { /* when is extern */
			/*	if(current_sentence->is_symbol==1)
					fprintf(stderr, "Warning in line %d: the line has both symbol and extern declaration\n", line_number); MOVING THIS TO LINE_PARSER.C */
				add_to_symbol_table(current_sentence->symbol, -999, 1, NONE);
			}
			else {
				if (current_sentence->is_symbol == 1) {
					if (!symbol_exists(current_sentence->symbol)) {
						add_to_symbol_table(current_sentence->symbol, IC, 0, CODE);
					}
					else
					{
						fprintf(stderr, "Error in line %d: symbol %s already exists in symbols table\n", line_number, current_sentence->symbol);
						syntax_errors = TRUE;
					}
				}
				/* analyzing sentence so IC is increased by the number of memory words the action sentence takes */
				if (current_sentence->is_action)
					increase_IC_according_sentence(current_sentence);
			}
		}

		if (sentence_head == NULL) { /* head to the parsed sentence list - so if we go through 2nd pass we won't need to parse the line again */
			sentence_head = current_sentence;
			sentence_tail = sentence_head;
		}
		else {
			sentence_tail->next = current_sentence;
			sentence_tail = sentence_tail->next;
		}
	}

	if (syntax_errors == TRUE) {
		free_data(data_head);
		free_symbol(symbol_head);
		free_sentence(sentence_head);
		return FALSE;
	}

			/* updating the DC address of each entry in symbol_table with the IC offset, only when 													the symbol is of DATA type */
			increase_DC_symbol_address_by_IC_offset();
			return TRUE;
}










