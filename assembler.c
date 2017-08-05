#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "number_conversions.h"
#include "line_parser.h"

#define OPCODES_TABLE_LENGTH size_opcode_table


char line[80];
char *current_word; 
char *current_symbol;
int last_position;
int syntax_errors = FALSE;
int IC; 
int DC;
int line_number;
symbol_line *symbol_tail = NULL;
symbol_line *symbol_head = NULL;
memory_word *data_tail = NULL;
memory_word *data_head = NULL;



/* is_current_word_empty - 
 receives: the current word obtained from list. 
 returns: 1 if the word is empty, 0 if not.
 NOTE: The used of this function is in order to handle empty spaces or tabs between words in the line from the source file. */ 
int is_current_word_empty(char *word)
{
	return strlen(word) == 0 ? TRUE : FALSE;
}

/* get_next_word - 
 receives: a pointer to the line to take the word from, a pointer to the current_word, and the position in the line where last time the function was called returned.   
 returns: the position where we stopped in the line. 
 NOTE: in case of ":" or "." - these will be returned as part of the word. The reason is that these characters will assist
 with symbol type and commands recognition. They must be removed afterwards, in other function.
 NOTE2: The parameter last_position should be -1 when a new line is parsed. */
int get_next_word(char *current_word, char *line, int last_position)
{	
 	int position = last_position+1;
	int i = 0;
	while(line[position] != ' ' && line[position] != ',' && line[position] != '[' && line[position] != ']')
		{
			current_word[i] = tolower(line[position]); /* to unify all comparisons so they will be executed in lower case */
			i++;
			position++;
		}
	current_word[i] = '\0';	

	if(is_current_word_empty(current_word))
		position = get_next_word(current_word, line, position);

	return position;
}



/* is_symbol - 
 receives: the current word to parse,
 returns: 1 if it's a symbol, 0 if not. */ 
int is_symbol(char *current_word)
{	
	int length = strlen(current_word);
	if (current_word[length-1] == ':' && length <=30)
		{ 
		  if(!isalpha(current_word[0])) {
			fprintf(stderr, "Error in line %d - symbol cannot start with a non alphabetic character\n", line_number);
			syntax_errors = TRUE;
			return FALSE;
		  }
		  current_word[length-1] = '\0';
		  return TRUE;
		}
	
	return FALSE;
}


/* is_store_command -
 receives: the current word to parse,
 returns: 1 if the word is .data/.string/.mat, 0 if not. */
int is_store_command(char *current_word)
{
	if(current_word[0] != '.')
		return FALSE;
	else
	   strncpy(current_word, current_word+1, strlen(current_word)-1); /* removes '.' from the beginning */
	
	current_word[strlen(current_word)-1] = '\0'; /* in order to strcmp to be accurate */
	   
	if (strcmp(current_word, "data") == 0 || strcmp(current_word, "mat") == 0 || strcmp(current_word, "string") == 0) 
		return TRUE;

	return FALSE;
}


/* is_extern_or_entry_command - 
 receives: the current word to parse,
 returns: 1 if the word is .extern/.entry, 0 if not. */
int is_extern_or_entry_command(char *current_word)
{
	if(current_word[0] != '.')
		return FALSE;
	else
	   strncpy(current_word, current_word+1, strlen(current_word)-1); /* removes '.' from the beginning */
	
	current_word[strlen(current_word)-1] = '\0'; /* in order to strcmp to be accurate */
	   
	if (strcmp(current_word, "extern") == 0 || strcmp(current_word, "entry") == 0) 
		return TRUE;
	
	return FALSE;

}

/* is_extern -
 receives: the current word after we detected it is "extern" or "entry",
 returns: 1 if it's "extern", 0 if not. 
*/
int is_extern(char *current_word)
{
	return (strcmp(current_word, "extern") == 0 ? TRUE : FALSE);

}


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
		
		binary_char = convert_ascii_value_to_binary(curr->string[i]);
		strcpy(new_memory_word->bits, binary_char); 
	
		if (data_tail)
			data_tail->next = new_memory_word; 
		else {
			data_head = new_memory_word;
			data_tail = data_head;
			}
		
		added_mem_words++; 
	}
		/* adding '\0' to data table at the end of the string */
		new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		strcpy(new_memory_word->bits, "00000000\0");
		
		added_mem_words++; 
		data_tail->next = new_memory_word; 
	
		return added_mem_words;
}


/* add_num_to_data_table - 
   receives the sentence after it was parsed and converts each number of the data_arr of the sentence into it's binary value.
   returns the number of memory words added to the data table. */
int add_num_to_data_table(sentence *curr)
{
	int i;
	memory_word* new_memory_word;
	int added_mem_words = 0;
	
	for(i=0; i < curr->data_arr_num_of_params+1 ; i++) {
	
	new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		if (!new_memory_word)
		{
			fprintf(stderr, "Memory allocation for new memory word failed!");
			exit(1);
		}
	
	strcpy(new_memory_word->bits, convert_dec_to_binary(curr->data_arr[i]));
	if (data_tail)
		data_tail->next = new_memory_word; 
	else {
		data_head = new_memory_word;
		data_tail = data_head;
		}
		
		added_mem_words++; 	 
	}
		
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
	
	for(i=0; i < (curr->mat_num_of_rows)*(curr->mat_num_of_cols)+1 ; i++) {
	
	new_memory_word = (memory_word*)malloc(sizeof(memory_word));
		if (!new_memory_word)
		{
			fprintf(stderr, "Memory allocation for new memory word failed!");
			exit(1);
		}
	
	strcpy(new_memory_word->bits, convert_dec_to_binary(curr->mat[i]));
	if (data_tail)
		data_tail->next = new_memory_word; 
	else {
		data_head = new_memory_word;
		data_tail = data_head;
		}
		
		added_mem_words++; 	 
	}
		
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
	if(strcmp(curr->source_operand_type,curr->source_operand_type)==0 && strcmp(curr->source_operand_type,"11")==0) {
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


void execute_first_pass(FILE *fd)
{ 
	sentence *current_sentence;	
	line_number = 0;	
	IC = 100;
	DC = 0;
		
	while(fgets(line,80,fd))
	{	
		line_number++;		
		current_sentence = parse_sentence(line, line_number);
		

		/* MOVE TO PARSE SENTENCE: 
		has_symbol = 0; 
		last_position = get_next_word(current_word, line, -1);
		*/

		if(current_sentence->is_store_command == TRUE) { 
			if(current_sentence->is_symbol) { 					
				if(!symbol_exists(current_sentence->symbol)) {
					add_to_symbol_table(current_sentence->symbol, DC, 0, DATA);
					}	
				else
					fprintf(stderr, "Error in line %d: symbol %s already exists in symbols table\n", line_number, 						current_sentence->symbol);
			}
				/* adding into data table. DC is increased: */ 
					add_to_data_table(current_sentence);				
		}

		else { /* when is_store_command = 0 */
			if(current_sentence->guidance_command == EXTERN) { /* when is extern */
				if(current_sentence->is_symbol==1)
					fprintf(stderr, "Warning in line %d: the line has both symbol and extern declaration\n", line_number);
				 add_to_symbol_table(current_sentence->symbol, -999, 1, NONE);
			} 
			else { 
				if(current_sentence->is_symbol==1) { 
					if(!symbol_exists(current_sentence->symbol)) {					
					add_to_symbol_table(current_symbol, IC, 0, CODE);
					}
					else
					fprintf(stderr, "Error in line %d: symbol %s already exists in symbols table\n", line_number, 							current_sentence->symbol);
				} 
				/* analyzing sentence so IC is increased by the number of memory words the action sentence takes */
				if(current_sentence->is_action)				
					increase_IC_according_sentence(current_sentence);
			} 
		}
			free(current_sentence);			
	
	}
			/* updating the DC address of each entry in symbol_table with the IC offset, only when 
													the symbol is of DATA type */
			increase_DC_symbol_address_by_IC_offset();
			
}


/* In assembler.h interface: run_assembler - calls the first and second pass */
int run_assembler(FILE *fd){
	
	execute_first_pass(fd);
       	return 0;

}






