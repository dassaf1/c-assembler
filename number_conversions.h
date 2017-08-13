#ifndef _NUMBER_CONVERSIONS_H_
#define _NUMBER_CONVERSIONS_H_

/* char * convert_binary_to_base_4_strange(char binary_address[10]); */

char * convert_base_4_to_base_4_strange(int num); /* DONE */
int convert_dec_to_another_base(int num, int base); 

void convert_ascii_value_to_10_bit_binary(char c, char * arr); /* DONE */
void convert_dec_to_x_bit_binary(int num, int bits, char * arr); /* DONE */


#endif
