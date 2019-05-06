#pragma once
#define BUF_SIZE 1000

int b64(char **argv, unsigned int n);

void chars_to_b64(unsigned char *buf_in, unsigned char *buf_out, unsigned int r);

void f_out(unsigned char *buf, unsigned int space, unsigned int n, FILE *f2, unsigned int mode);

int
b64_to_chars(unsigned char *buf_in, unsigned char *buf_out, unsigned int ign, unsigned int n, unsigned int *new_length);

void encode(FILE *f1, FILE *f2, unsigned int space);

unsigned int decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space);

void print_help(void);