#ifndef _NUMBER_CONVERSIONS_H_ 
#define _NUMBER_CONVERSIONS_H_

void convert_binary_to_base_4_strange(char binary_address[11], char * result);
void convert_base_4_to_base_4_strange(int num, char * result);
int convert_dec_to_another_base(int num, int base);
void convert_dec_to_base_4_strange(int num, char * result);
void convert_ascii_value_to_10_bit_binary(char c, char * result);
void convert_dec_to_x_bit_binary(int num, int bits, char * result);

#endif 
