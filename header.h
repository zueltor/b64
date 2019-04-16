#pragma once
void char_to_ind(unsigned char *buf, unsigned int i);
void ind_to_char(unsigned char *buf);
void ind_to_b64(unsigned char *buf, unsigned int r);
void b64_to_ind(unsigned char *c);
void encode(FILE *f1, FILE *f2, unsigned int space);
void help(void);
unsigned int decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space);