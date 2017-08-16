#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "number_conversions.h"

#define WORKING_BASE 4

typedef struct s4_strange_entry {
    int digit;
    char character[2];
}s4_strange_entry;

typedef struct binary_to_s4_strange {
    char binary[3];
    char character[2];
}binary_to_s4_strange;

s4_strange_entry s4t [] = {
        {0, "a"},
        {1, "b"},
        {2, "c"},
        {3, "d"}
};

binary_to_s4_strange bin_to_4_strange [] = {
        {"00", "a"},
        {"01", "b"},
        {"10", "c"},
        {"11", "d"}
};

int strange_4_table_size = sizeof(s4t)/sizeof(s4t[0]);
int bin_to_4_strange_table_size = sizeof(bin_to_4_strange)/sizeof(bin_to_4_strange[0]);

/* Converts a digit to a strange 4 character */
char * digit_to_char_strange_4(int digit){
    int i;
    for (i=0; i<strange_4_table_size; i++){
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

/* Converts a decimal number to base 4 strange. */
void convert_dec_to_base_4_strange(int num, char * result) {
    convert_base_4_to_base_4_strange(convert_dec_to_another_base(num, WORKING_BASE), result);
}

/* Converts a decimal number to a binary number with padded zeroes up to x bits.
* When calling the function array of chars sized number of bits + 1 (for '\0') should be provided (char * result) */
void convert_dec_to_x_bit_binary(int num, int bits, char * result){
    int i = bits - 1;
    int binary_num = convert_dec_to_another_base(num, 2);

    result[i] = '\0';
    i--;
    do {
        result[i] = binary_num % 10 == 0 ? '0' : '1';
        binary_num = binary_num / 10;
        i--;
    }
    while (binary_num);

    for (; i >= 0; i--) { /* pad with zeros */
        result[i] = '0';
    }
}

/* Coverts an alphabetic char into its binary value using 10 bits (using zero padding)
* When calling the function array of chars sized number of bits + 1 (for '\0') should be provided (char * result) */
void convert_ascii_value_to_10_bit_binary(char c, char * result){
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
        fprintf(stderr, "ERROR: char is not an alphabetic character");
    }
    convert_dec_to_x_bit_binary(c, 11, result);
}

/* Helper function to convert_base_4_to_base_4_strange. */
void convert_base_4_to_base_4_strange_internal(int num, char * result){
    if (num < 10 && num >= 0) {
        strcpy(result, digit_to_char_strange_4(num));
        return;
    }
    convert_base_4_to_base_4_strange_internal(num / 10, result);
    strcat(result, digit_to_char_strange_4(num % 10));
}

/* converts a number in base 4 to a number in base 4 strange. */
void convert_base_4_to_base_4_strange(int num, char * result){
    convert_base_4_to_base_4_strange_internal(num, result);
    strcat(result, "\0");
}

void convert_binary_to_base_4_strange(char binary_address[11], char * result) {
    int i, k;
    char two_chars[3];
    for (i = 0; i < 10; i+=2) {
        strncpy(two_chars, binary_address+i, 2);
        two_chars[2] = '\0';
        for (k=0; k < bin_to_4_strange_table_size; k++) {
            if (strcmp(two_chars, bin_to_4_strange[k].binary) == 0) {
                if (i == 0) {
                    strcpy(result, bin_to_4_strange[k].character);
                }
                else {
                    strcat(result, bin_to_4_strange[k].character);
                }
                break;
            }
        }
    }
    strcat(result, "\0");
}