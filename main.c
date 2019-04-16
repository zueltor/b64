#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void char_to_ind(unsigned char *buf, unsigned int i);
void ind_to_char(unsigned char *buf);
void ind_to_b64(unsigned char *buf, unsigned int r);
void b64_to_ind(unsigned char *c);
void encode(FILE *f1, FILE *f2, unsigned int space);
void help(void);
unsigned int decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space);

int main(int argc, char *argv[])
{
	unsigned int i,
		err,
		ign;
	int space;

	if ((argc > 7) || (argc < 4))
	{
		printf("bad input\n");
		help();
		return 1;
	}

	FILE *f1 = fopen(argv[argc - 2], "rb");
	FILE *f2 = fopen(argv[argc - 1], "wb");

	if (f1 == NULL)
	{
		printf("error %s\n", argv[argc - 2]);
		help();
		return 1;
	}
	if (f2 == NULL)
	{
		printf("error %s\n", argv[argc - 1]);
		help();
		return 1;
	}

	err = 0;
	i = argc - 3;
	space = 0;
	ign = 0;
	if (space = atoi(argv[i]))
	{
		if (!strcmp(argv[i - 1], "-f") && (space > 0))
		{
			i -= 2;
		}
		else
			err = 1;
	}

	if (!strcmp(argv[i], "-e") && i == 1)
	{
		encode(f1, f2, space);
	}
	else
	if (!strcmp(argv[i], "-d"))
	{
		if (!strcmp(argv[1], "-i") && i == 2)
			ign = 1;
		else if (i != 1)
			err = 1;
		if (decode(f1, f2, ign, space))
		{
			printf("file has wrong symbol(s)");
			return 1;
		}
	}
	else
		err = 1;
	if (err)
		help();

	fclose(f1);
	fclose(f2);
	return 0;

}


