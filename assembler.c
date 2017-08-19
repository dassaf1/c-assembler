#include <stdlib.h>
#include "data_structures.h"
#include "assembler.h"

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
}

/* code -
receives: the head of the code list and free memory. */
void free_code(memory_word *code_head)
{
    memory_word *temp;

    while (code_head)
    {
        temp = code_head->next;
        free(code_head);
        code_head = temp;
    }
}

/* free all linked lists used by assembler */
void free_second_pass_data_structs_linked_lists() {
    free_symbol(symbol_head);
    free_sentence(sentence_head);
    free_code(code_head);
}