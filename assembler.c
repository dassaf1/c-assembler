#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"

#define OPCODES_TABLE_LENGTH size_opcode_table

char line[80];
char *current_word; 
char *current_symbol;
int last_position;
int IC = 100; 
int DC = 0;
symbol_line *symbol_tail = NULL;
symbol_line *symbol_head = NULL;
memory_word *data_tail = NULL;
memory_word *data_head = NULL;

/* TODO: set global flag for errors and implement it where relevant */


/* is_current_word_empty - 
 receives: the current word obtained from list. 
 returns: 1 if the word is empty, 0 if not.
 NOTE: The used of this function is in order to handle empty spaces or tabs between words in the line from the source file. */ 
int is_current_word_empty(char *word)
{
	return strlen(word) == 0 ? 1 : 0;
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
	if(!isalpha(current_word[0]))
		/* TODO: add print error to stderr and turn on flag */ 
		return 0;
	if (current_word[length-1] == ':' && length <=30)
		{ 
		  current_word[length-1] = '\0';
		  return 1;
		}
	
	return 0;
}


/* is_store_command -
 receives: the current word to parse,
 returns: 1 if the word is .data/.string/.mat, 0 if not. */
int is_store_command(char *current_word)
{
	if(current_word[0] != '.')
		return 0;
	else
	   strncpy(current_word, current_word+1, strlen(current_word)-1); /* removes '.' from the beginning */
	
	current_word[strlen(current_word)-1] = '\0'; /* in order to strcmp to be accurate */
	   
	if (strcmp(current_word, "data") == 0 || strcmp(current_word, "mat") == 0 || strcmp(current_word, "string") == 0) 
		return 1;

	return 0;
}


/* is_extern_or_entry_command - 
 receives: the current word to parse,
 returns: 1 if the word is .extern/.entry, 0 if not. */
int is_extern_or_entry_command(char *current_word)
{
	if(current_word[0] != '.')
		return 0;
	else
	   strncpy(current_word, current_word+1, strlen(current_word)-1); /* removes '.' from the beginning */
	
	current_word[strlen(current_word)-1] = '\0'; /* in order to strcmp to be accurate */
	   
	if (strcmp(current_word, "extern") == 0 || strcmp(current_word, "entry") == 0) 
		return 1;
	
	return 0;

}

/* is_extern -
 receives: the current word after we detected it is "extern" or "entry",
 returns: 1 if it's "extern", 0 if not. 
*/
int is_extern(char *current_word)
{
	return (strcmp(current_word, "extern") == 0 ? 1 : 0);

}


/* symbol_exists - 
 receives: the head of the symbols table, the current symbol. The function searches within the list if the symbol already exists.
 returns: 1 if already exists, 0 if not. */
int symbol_exists(symbol_line *head, char *current_symbol) 
{
	symbol_line *temp = head; 
	while (temp)
	{
		if (strcmp(temp->symbol, current_symbol) == 0) 
		return 1; 

		else
			temp = temp->next;
	}
	
	return 0;
}

/* add_to_symbol_table -
 receives: the last line in the symbols table, the current symbol to be added, the address where it's going to be allocated (DC or 0 if it's extern),
 1 if the symbol is extern / 0 if not, the symbol type (from enum: NONE, DATA, CODE). The function allocates a new node
 of type symbol_line to the symbols table and returns a pointer to this last added. */  
symbol_line* add_to_symbol_table(symbol_line *tail, char* current_symbol, int address, int is_extern, int symbol_type)
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

	if(!tail)
		tail = new_symbol;
	else {
		tail->next = new_symbol;
		tail = tail->next;
	}

	return tail;

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
			return 1;
	}
	
	return 0;
}

/* To complete: 
a) detect the type of data to convert to data table -> validate following input -> convert -> add line to data table\list.
b) detect the type of opcode -> validate following input -> convert -> add line to code table\list. */

void execute_first_pass(FILE *fd)
{ 
	int line_number = 0;
	sentence *current_sentece;
		
	while(fgets(line,80,fd))
	{	
		current_sentence = parse_sentence(line);
		line_number++;

		/* MOVE TO PARSE SENTENCE: 
		has_symbol = 0; 
		last_position = get_next_word(current_word, line, -1);
		*/

		if(current_sentence->is_store_command == 1) { /*open b*/
			if(current_sentece->is_symbol) { /*open a*/					
				if(!symbol_exists(symbol_head, current_sentence->symbol)) {
					symbol_tail = add_to_symbol_table(symbol_tail, current_sentence->symbol, DC, 0, DATA);
					}	
				else
					fprintf(stderr, "Error in line %d: symbol %s already exists in symbols table\n", line_number, 						current_sentece->symbol);
			}/*close a*/
				/*TODO: add into data table: 
				   TODO: ask current_sentence for types and insert into data table, increase DC */				
		}/*close b*/

		else { /*open c*/ /* when is_store_command = 0 */
			if(strcmp(current_sentence->guidance_command,"extern")==0) { /*open d*/ /* when is extern */
				if(current_sentence->is_symbol==1)
					fprintf(stderr, "Warning in line %d: the line has both symbol and extern declaration\n");
				 symbol_tail = add_to_symbol_table(symbol_tail, current_sentence->symbol, -999, 1, NONE);
			} /*close d*/
			else { /*open e*/ 
				if(current_sentence->is_symbol==1) { /*open f*/
					if(!symbol_exists(symbol_head, current_sentence->symbol)) {					
					symbol_tail = add_to_symbol_table(symbol_tail, current_symbol, IC, 0, CODE);
					}
					else
					fprintf(stderr, "Error in line %d: symbol %s already exists in symbols table\n", line_number, 							current_sentece->symbol);
				} /*close f*/
				/* TODO: add increase of IC according the current sentece */
			} /*close c*/
	}
			/* TODO: go over the symbol table and update the address of each entry with the IC offset, only when 
				the symbol is of type DATA */
		
}


/* In assembler.h interface: run_assembler - calls the first and second pass */
int run_assembler(FILE *fd){
	
	execute_first_pass(fd);
	return 0;

}






