#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"

char line[80];
char *current_word;
char *current_symbol;
int last_position;
int IC = 116;
int DC = 12;

void execute_second_pass(FILE *fd) {
    int is_error_found = 0;
    int ic = 0;
	int line_number = 0;
	sentence *current_sentece;

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
