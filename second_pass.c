#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "number_conversions.h"


#define ABSOLUTE_ARE "00"
#define EXTERNAL_ARE "01"
#define RELOCATABLE_ARE "10"
#define IMMEDIATE_DELIVERY_METHOD "00" /* should to be GLOBAL */
#define DIRECT_DELIVERY_METHOD "01" /* should to be GLOBAL */
#define MATRIX_DELIVERY_METHOD "10" /* should to be GLOBAL */
#define REGISTER_DELIVERY_METHOD "11" /* should to be GLOBAL */
#define CODE_TABLE_START_ADDRESS 100 /* should to be GLOBAL */
#define MAX_FILE_NAME_SIZE 100 /* should to be GLOBAL */
#define BASE_4_STRAGE_NUMBER_MAX_DIGITS 10
#define INPUT_FILE_MAX_LINE_SIZE 81 /* should to be GLOBAL */
#define OBJECT_FILE_EXTENSION ".ob"
#define EXTERN_FILE_EXTENSION ".ext"
#define ENTRY_FILE_EXTENSION ".ent"

FILE * input_fd;
FILE * object_fd;
FILE * extern_fd;
FILE * intern_fd;
char input_filename[MAX_FILE_NAME_SIZE];
char object_filename[MAX_FILE_NAME_SIZE];
char extern_filename[MAX_FILE_NAME_SIZE];
char intern_filename[MAX_FILE_NAME_SIZE];

int IC = 116; /* DATA received from first pass */
int DC = 12; /* DATA received from first pass */
/*sentence * sentence_head;  DATA received from first pass */
symbol_line * symbol_head = NULL; /*  DATA received from first pass */
memory_word * data_head = NULL; /*  DATA received from first pass */

int ic_second_pass = CODE_TABLE_START_ADDRESS;
int is_error_found = 0;
int line_number = 1;

memory_word * code_head = NULL;
memory_word * code_tail = NULL;

void encode_instruction(sentence * curr);
void encode_immediate_operand(int oper);
void encode_direct_operand(char * oper);
void encode_extern_operand();
void encode_register(char * register_names[], char * operand_type);
int encode_matrix(char * mat_oper, char * matrix_row_operand, char * matrix_col_operand);
int handle_direct_and_matrix_operands(char * oper);
void add_item_to_code_list(memory_word * item);
void append_data_table_into_end_of_code_table(memory_word * data_head);
void execute_second_pass(char * filename);

void print_memory_word_list(memory_word * head) {
    memory_word * tmp = head;
    while (tmp) {
        printf("Address: %d, bits: %s\n", tmp->address, tmp->bits);
        tmp = tmp->next;
    }
}

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
		fprintf(stderr, "ERROR: Memory allocation failed");
		is_error_found = 1;
		return NULL;
	}
	return new_memory_word;
}

symbol_line * get_symbol_line_from_symbol_table(char * symbol) {
    symbol_line * sl = symbol_head;
    while (sl) {
        if (strcmp(sl->symbol, symbol) == 0) {
            return sl;
        }
        sl = sl->next;
    }
    fprintf(stderr, "ERROR: Operand %s is not defined in symbol table (line %d)\n", symbol, line_number);
	is_error_found = 1;
	return NULL;
}

/* Encode direct operand into code table */
void encode_direct_operand(char * oper) {
    char arr[9];
    memory_word * new_memory_word = create_new_memory_word();
    symbol_line * sl = get_symbol_line_from_symbol_table(oper);
    if (sl) {
        new_memory_word->address = ic_second_pass;
        convert_dec_to_x_bit_binary(sl->address, 9, arr);
        strcpy(new_memory_word->bits, arr);
        strcat(new_memory_word->bits, RELOCATABLE_ARE);
        add_item_to_code_list(new_memory_word);
        ic_second_pass++;
    }
}

/* Encode immediate operand into code table */
void encode_immediate_operand(int oper) {
    char arr[9];
    memory_word * new_memory_word = create_new_memory_word();
    new_memory_word->address = ic_second_pass;
    convert_dec_to_x_bit_binary(oper, 9, arr);
    strcpy(new_memory_word->bits, arr);
    strcat(new_memory_word->bits, ABSOLUTE_ARE);
    add_item_to_code_list(new_memory_word);
    ic_second_pass++;
}

/* Encode external operand into code table */
void encode_extern_operand() {
    memory_word * new_memory_word = create_new_memory_word();
    new_memory_word->address = ic_second_pass;
    strcpy(new_memory_word->bits, "00000000");
    strcat(new_memory_word->bits, EXTERNAL_ARE);
    add_item_to_code_list(new_memory_word);
    ic_second_pass++;
}

/* Encode matrix into code table */
int encode_matrix(char * mat_oper, char * matrix_row_operand, char * matrix_col_operand) {
    char * regs[3];
    regs[0] = matrix_row_operand;
    regs[1] = matrix_col_operand;
    if (!(handle_direct_and_matrix_operands(mat_oper))) {
        return 0;
    }
    encode_register(regs, "both");
    return 1;
}

/* Encode registers into code table. operand_type should be one of source, destination or both. If both is specified,
 both source and destination registers will be encoded into the same memory word */
void encode_register(char * register_names[], char * operand_type) {
    int i;
    char register1_val[4];
    char register2_val[4];
    memory_word * new_memory_word = create_new_memory_word();
    new_memory_word->address = ic_second_pass;

    if (! register_names[0]) {
        fprintf(stderr, "ERROR: register_names array is empty");
        is_error_found = 1;
        return;
    }
    for(i = 0; i < size_registers_table; i++) {
        if (strcmp(registers_table[i].register_name, register_names[0]) == 0) {
            strcpy(register1_val, registers_table[i].register_val);
        }
        if (register_names[1]) {
            if (strcmp(registers_table[i].register_name, register_names[1]) == 0) {
                strcpy(register2_val, registers_table[i].register_val);
            }
        }
    }

    if (strcmp(operand_type, "source") == 0) {
        strcpy(new_memory_word->bits, register1_val);
        strcat(new_memory_word->bits, "0000");
    }
    else {
        if (! register_names[1]) {
            fprintf(stderr, "ERROR: register_names array position 1 is empty\n");
            is_error_found = 1;
            return;
        }
        if (strcmp(operand_type, "destination") == 0) {
            strcpy(new_memory_word->bits, "0000");
            strcat(new_memory_word->bits, register2_val);
        }
        else if (strcmp(operand_type, "both") == 0) { /* encode both source and destination operands */
            strcpy(new_memory_word->bits, register1_val);
            strcat(new_memory_word->bits, register2_val);
        }
    }
    strcat(new_memory_word->bits, ABSOLUTE_ARE);
    add_item_to_code_list(new_memory_word);
    ic_second_pass++;
}

/* Encode instruction sentence into code table */
void encode_instruction(sentence * curr) {
    int i;
    memory_word * new_memory_word = create_new_memory_word();
	new_memory_word->address = ic_second_pass;
	for(i = 0; i < size_opcode_table; i++) {
        if (strcmp(opcodes_table[i].opcode, curr->opcode) == 0) {
            strcpy(new_memory_word->bits, opcodes_table[i].binary_val);
        }
    }
    strcat(new_memory_word->bits, curr->source_operand_type);
    strcat(new_memory_word->bits, curr->dest_operand_type);
    strcat(new_memory_word->bits, ABSOLUTE_ARE); /* Instruction is always type absolute */
    add_item_to_code_list(new_memory_word);
    ic_second_pass++;
}

/* Will update memory addresses of data items in data tables to the addresses after the code part in the code table.
 * In addition will connect the last node in the code linked list to the first node of the data linked list */
void append_data_table_into_end_of_code_table(memory_word * data_head) {
    memory_word * tmp = data_head;
    while(tmp) {
        tmp->address = ic_second_pass;
        tmp = tmp->next;
        ic_second_pass++;
    }
    code_tail->next = data_head;
}

void add_line_to_file(FILE * fd, char * column_a, char * column_b) {
    fprintf(fd, "%s    %s\n", column_a, column_b);
}

/* Will append to extern file and encode external operand into code table if operand is external,
otherwise encode as direct operand. */
int handle_direct_and_matrix_operands(char * oper) {
    symbol_line * sl;
    if (! (sl = get_symbol_line_from_symbol_table(oper))) {
        return 0;
    }
    if (sl->is_extern) {
        char conversion_result[BASE_4_STRAGE_NUMBER_MAX_DIGITS];
        convert_dec_to_base_4_strange(ic_second_pass, conversion_result);
        add_line_to_file(extern_fd, oper, conversion_result);
        encode_extern_operand();
    }
    else{
        encode_direct_operand(oper);
    }
    return 1;
}

void create_object_file() {
    memory_word * tmp = code_head;
    char conversion_result_1[BASE_4_STRAGE_NUMBER_MAX_DIGITS];
    char conversion_result_2[BASE_4_STRAGE_NUMBER_MAX_DIGITS];
    /* write DC and IC size in 4 strange */
    convert_dec_to_base_4_strange(DC, conversion_result_1);
    convert_dec_to_base_4_strange(IC-CODE_TABLE_START_ADDRESS, conversion_result_2);
    add_line_to_file(object_fd, conversion_result_1, conversion_result_2);
    /* write code and data table in 4 strange */
    while (tmp) {
        convert_dec_to_base_4_strange(tmp->address, conversion_result_1);
        convert_binary_to_base_4_strange(tmp->bits, conversion_result_2);
        add_line_to_file(object_fd, conversion_result_1, conversion_result_2);
        tmp = tmp->next;
    }
}

void combine_filename_with_new_file_extension(char * filename, char * output_filename, char * extension) {
    char * filename_prefix;
    char extension_filename[MAX_FILE_NAME_SIZE];
    char copy_of_filename[MAX_FILE_NAME_SIZE];
    strcpy(copy_of_filename, filename);
    filename_prefix = strtok(copy_of_filename, ".");
    strcpy(extension_filename, filename_prefix);
    strcat(extension_filename, extension);
    strcpy(output_filename, extension_filename);
}

int open_second_pass_output_files() {
    input_fd = fopen(input_filename, "r");
    if (!input_fd) {
        fprintf(stderr, "ERROR: Could not open input file named: %s\n", input_filename);
        return 0;
    }
    object_fd = fopen(object_filename, "w");
    extern_fd = fopen(extern_filename, "w");
    intern_fd = fopen(intern_filename, "w");
    return 1;
}

void close_second_pass_output_files() {
    fclose(input_fd);
    fclose(object_fd);
    fclose(extern_fd);
    fclose(intern_fd);
}

void execute_second_pass(char * filename) {

    /*sentence * current_sentence = sentence_head;    ENABLE THIS AFTER DELETION OF FOR TEST BLOCK!! */

    /* FOR TEST */
    sentence * sent1 = (sentence *)malloc(sizeof(sentence));
    sentence * sent2 = (sentence *)malloc(sizeof(sentence));
    sentence * sentence_head = sent1;
    sentence * current_sentence = sentence_head;
    sent1->is_action = 1;
    strcpy(sent1->opcode, "stop");
    strcpy(sent1->symbol, "W");
    sent1->num_of_operands = 2;
    strcpy(sent1->source_operand_type, "10");
    strcpy(sent1->dest_operand_type, "10");
    strcpy(sent1->operand_1, "W");
    strcpy(sent1->operand_2, "W");
    strcpy(sent1->matrix_row_operand_a, "r1");
    strcpy(sent1->matrix_col_operand_a, "r2");
    strcpy(sent1->matrix_row_operand_b, "r1");
    strcpy(sent1->matrix_col_operand_b, "r2");
    sent1->guidance_command = ENTRY;
    sent1->immediate_operand_a = 5;
    sent1->immediate_operand_b = 7;
    sent1->next = sent2;

    /* END FOR TEST */

    strcpy(input_filename, filename);
    combine_filename_with_new_file_extension(input_filename, object_filename, OBJECT_FILE_EXTENSION);
    combine_filename_with_new_file_extension(input_filename, extern_filename, EXTERN_FILE_EXTENSION);
    combine_filename_with_new_file_extension(input_filename, intern_filename, ENTRY_FILE_EXTENSION);
    if (!open_second_pass_output_files()) {
        return;
    }

    /* Second pass - actual encoding of the code table */
    while(current_sentence) {
		if(current_sentence->is_action == 0) { /* is guidance sentence */
		    if (current_sentence->guidance_command == ENTRY) {
                char conversion_result[BASE_4_STRAGE_NUMBER_MAX_DIGITS];
                symbol_line * sl = get_symbol_line_from_symbol_table(current_sentence->symbol);
                if (!sl) {
                    break;
                }
                convert_dec_to_base_4_strange(sl->address, conversion_result);
                add_line_to_file(intern_fd, current_sentence->symbol, conversion_result);
		    }
		}
		else { /* is instruction sentence */
		    encode_instruction(current_sentence);
            if (current_sentence->num_of_operands == 2) { /* instruction sentence with 2 operands */
                /* encode source operand */
                if (strcmp(current_sentence->source_operand_type, IMMEDIATE_DELIVERY_METHOD) == 0) {
                    encode_immediate_operand(current_sentence->immediate_operand_a);
                }
                else if (strcmp(current_sentence->source_operand_type, DIRECT_DELIVERY_METHOD) == 0) {
                    if (! (handle_direct_and_matrix_operands(current_sentence->operand_1))) {
                        break;
                    }
                }
                else if (strcmp(current_sentence->source_operand_type, MATRIX_DELIVERY_METHOD) == 0) {
                    if (!(encode_matrix(current_sentence->operand_1, current_sentence->matrix_row_operand_a, current_sentence->matrix_col_operand_a))) {
                        break;
                    }
                }
                else { /* is register delivery method */
                    char * register_names[2];
                    register_names[0] = current_sentence->operand_1;
                    encode_register(register_names, "source");
                }
            }
            if (current_sentence->num_of_operands == 1) { /* instruction sentence with 1 operand */
            /* encode destination operand */
                if (strcmp(current_sentence->dest_operand_type, IMMEDIATE_DELIVERY_METHOD) == 0) {
                    encode_immediate_operand(current_sentence->immediate_operand_b);
                }
                else if (strcmp(current_sentence->dest_operand_type, DIRECT_DELIVERY_METHOD) == 0) {
                    if (! (handle_direct_and_matrix_operands(current_sentence->operand_2))) {
                        break;
                    }
                }
                else if (strcmp(current_sentence->dest_operand_type, MATRIX_DELIVERY_METHOD) == 0) {
                    if (!(encode_matrix(current_sentence->operand_2, current_sentence->matrix_row_operand_b, current_sentence->matrix_col_operand_b))) {
                        break;
                    }
                }
                else { /* is register delivery method */
                    char * register_names[2];
                    register_names[0] = current_sentence->operand_2;
                    encode_register(register_names, "source");
                }
            }
		}
		line_number++;
		current_sentence = current_sentence->next;
	}

    if (is_error_found) {
        fprintf(stderr, "ERROR: Errors found in second pass. Output files will not be created.\n");
        close_second_pass_output_files();
        return;
    }

    append_data_table_into_end_of_code_table(data_head);

    create_object_file();
    close_second_pass_output_files();
}

int main() {
/*  TESTING BLOCK **********************************
 *
 *  char * regs[3] = {"r1", "r6"};

    sentence * curr = (sentence *)malloc(sizeof(sentence));
    symbol_line * sl1 = (symbol_line*)malloc(sizeof(symbol_line));
    symbol_line * sl2 = (symbol_line*)malloc(sizeof(symbol_line));
    symbol_line * sl3 = (symbol_line*)malloc(sizeof(symbol_line));

    memory_word * mw1 = (memory_word*)malloc(sizeof(memory_word));
    memory_word * mw2 = (memory_word*)malloc(sizeof(memory_word));

    symbol_head = sl1;
    sl1->next = sl2;
    sl2->next = sl3;

    data_head = mw1;
    mw1->next = mw2;
    mw1->address = 0;
    strcpy(mw1->bits, "0000000001");
    mw2->address = 1;
    strcpy(mw2->bits, "0000000002");

    strcpy(sl2->symbol, "W");
    sl2->address = 8;

    strcpy(curr->opcode, "stop");
    strcpy(curr->source_operand_type, DIRECT_DELIVERY_METHOD);
    strcpy(curr->dest_operand_type, MATRIX_DELIVERY_METHOD);
    encode_instruction(curr);
    encode_immediate_operand(30);
    encode_direct_operand("W");
    encode_register(regs, "destination");
    encode_matrix("W", "r1", "r2");
    encode_extern_operand();

    print_memory_word_list(code_head);
    printf("\nic_second_pass: %d\n\n", ic_second_pass);

    append_data_table_into_end_of_code_table(data_head);
    print_memory_word_list(data_head);
    printf("\nic_second_pass after data append: %d\n\n", ic_second_pass);

    END OF TESTING BLOCK *********************************
    */

    /*  TESTING BLOCK ********************************** */

    symbol_line * sl1 = (symbol_line*)malloc(sizeof(symbol_line));
    symbol_line * sl2 = (symbol_line*)malloc(sizeof(symbol_line));
    symbol_line * sl3 = (symbol_line*)malloc(sizeof(symbol_line));

    symbol_head = sl1;
    sl1->next = sl2;
    sl2->next = sl3;

    strcpy(sl2->symbol, "W");
    sl2->address = 8;
    sl2->is_extern = 1;

    char filename[] = "file.as";
    execute_second_pass(filename);

    print_memory_word_list(code_head);

    return 0;

    /* END OF TESTING BLOCK ********************************* */
}