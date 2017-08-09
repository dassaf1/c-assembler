#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"


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


/* is_saved_word -
   receives: the current word to check if it is a saved word in the language or not.
   returns 1 if the word is a saved word, 0 if not. */
int is_saved_word(char *current_word, int line_number, int *syntax_error)
{

	/* TODO: complete the function. */
	printf("Error in line %d - the word %s is a saved word in the language\n", line_number, current_word);
	*syntax_error = TRUE;
}				

/* is_symbol - 
 receives: the current word to parse,
 returns: 1 if it's a symbol, 0 if not. */ 
int is_symbol(char *current_word, int line_number, int *syntax_error)
{	
	int length = strlen(current_word);
	if (current_word[length-1] == ':' && length <=30)
		{ 
		  if(!isalpha(current_word[0])) {
			fprintf(stderr, "Error in line %d - symbol cannot start with a non alphabetic character\n", line_number);
			*syntax_errors = TRUE;
			return FALSE;
		  }
		  if(is_saved_word(current_word, line_number, syntax_error))
			return FALSE;

		  current_word[length-1] = '\0';
		  return TRUE;
		}
	else
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

/* parse_sentence - 
   receives: the line to be parsed and the current line number.
   The functions parses the line and populates the sentence structure (defined in data_structures.h) with the relevant fields.
   returns: a sentence structure. */ 
sentence * parse_sentence(char *line, int line_number, int *syntax_error) {
	
	int last_position;
	char *current_word;
	int opc;

	sentence *parsed = (sentence*)malloc(sizeof(sentence));

	if (!parsed)
		{
			fprintf(stderr, "Memory allocation for new parsed sentence failed!");
			exit(1);
		}

	parsed->is_symbol = 0;
	parsed->is_action = 0;
	 
	last_position = get_next_word(current_word, line, -1);
	if(is_symbol(current_word, line_number, &syntax_error)) {  /* if so, set is_symbol to 1, copy it to current_symbol and get the  

							next word into current_word */
					
			parsed->is_symbol = 1; 			
			strcpy(parsed->symbol,current_word);
			last_position = get_next_word(current_word,line,last_position);
			
	}

	if(is_store_command(current_word)) {
		parsed->is_store_command = 1;
		detect_data_type(parsed,current_word);/* TODO - this functions */
		parse_data_by_its_type(parsed,current_word,last_position); /* check if sending parsed only is ok or if &parsed need to be 	                                                                         sent */
		}

	else if(is_extern_or_entry_command(current_word)) {
		parsed->is_store_command = 0;
		if(strcmp(current_word,"extern")==0)
			parsed->guidance_command = EXTERN;
		else
			parsed->guidance_command = ENTRY;
		
		last_position = get_next_word(current_word,line,last_position);
		
		if(is_symbol(current_word, line_number, &syntax_error)) { /* TODO: add parameter to is_symbol to set if the
":" must be a parameter to check symbol existance by */
			if(parsed->is_symbol==1) 
			 	fprintf(stderr, "Warning in line %d: the line has both symbol and extern declaration\n", line_number);
			strcpy(parsed->symbol,current_word);
		}
	}
	
	/* if first word is not symbol||store_cmd||extern or if first is symbol and second is not store_cmd||extern:*/
	opc = find_valid_opcode(current_word, last_position, line_number, &syntax_error);
	else if(opc > 0)
		{
		   parsed->is_action = 1;
	    	   strcpy(parsed->opcode,current_word);
		   
		   
				
			
				
	
		
				

	

	
		


	return parsed; 

}
