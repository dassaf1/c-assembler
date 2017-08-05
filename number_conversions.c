#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRANGE_4_TABLE_SIZE 4

typedef struct s4_strage_entry {
	int digit;
	char * character;
}s4_strage_entry;

s4_strage_entry s4t [] = {
	{0, "a"},
	{1, "b"},
	{2, "c"},
	{3, "d"}
};

/* Converts a digit to a strange 4 character */
char * digit_to_char_strange_4(int digit){
    int i;
    for (i=0; i<STRANGE_4_TABLE_SIZE; i++){
        if (digit == s4t[i].digit) {
            return s4t[i].character;
        }
    }
    fprintf(stderr, "Error: digit provided is not in 4 strange table\n");
    return '\0';
}

/* Converts a decimal number to any other base. */
int convert_dec_to_another_base(int number,int base){
    if(number == 0 || base==10)
        return number;
    return (number % base) + 10*convert_dec_to_another_base(number / base, base);
}

/* Converts a decimal number to a binary number with padded zeroes up to x bits. */
void convert_dec_to_x_bit_binary(int num, int bits, char * arr){
    int i = bits - 1;
    int binary_num = convert_dec_to_another_base(num, 2);
    do {
        arr[i] = binary_num % 10 == 0 ? '0' : '1';
        binary_num = binary_num / 10;
        i--;
    }
    while (binary_num);

    for (; i >= 0; i--) { /* pad with zeros */
        arr[i] = '0';
    }
}

/* Helper function to convert_base_4_to_base_4_strange. */
void convert_base_4_to_base_4_strange_internal(int num, char * result){
    if (num < 10 && num >= 0) {
        strcat(result, digit_to_char_strange_4(num));
        return;
    }
    convert_base_4_to_base_4_strange_internal(num / 10, result);
    strcat(result, digit_to_char_strange_4(num % 10));
}

/* converts a number in base 4 to a number in base 4 strange. */
char * convert_base_4_to_base_4_strange(int num){
    char * result = (char *)malloc(sizeof(char)*5);
    int i = num;
    int digits = 0;
    while (i != 0) {
        i = i / 10;
        digits++;
    }
    if (digits > 4) {
        printf("Max number of digits supported in base 4 strange are 4\n");
        return "\0";
    }
    convert_base_4_to_base_4_strange_internal(num, result);
    result[digits] = '\0';
    return result;
}