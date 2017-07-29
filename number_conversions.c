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

int convert_dec_to_another_base(int number,int base){
    if(number == 0 || base==10)
        return number;
    return (number % base) + 10*convert_dec_to_another_base(number / base, base);
}

char * convert_base_4_to_base_4_strange(int num){
    char dest[4] = "";
    if (num < 10 && num >= 0) {
        return digit_to_char_strange_4(num);
    }
    strcpy(dest, convert_base_4_to_base_4_strange(num / 10));
    printf("dest: %s\n", dest);
    return strcat(dest, digit_to_char_strange_4(num % 10));
}

int main() {
    printf("answer: %s\n", convert_base_4_to_base_4_strange(123));
    return 0;
}