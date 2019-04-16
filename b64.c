#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void char_to_ind(unsigned char *buf, unsigned int i)
{
	if (i == 3)
	{
		buf[3] = buf[2] & 0x3F;
		buf[2] = buf[2] >> 6;
	}
	else
	{
		buf[3] = 64;
		buf[2] = 0;
	}
	if (i >= 2)
	{
		buf[2] = buf[2] + ((buf[1] << 2) & 0x3C);
		buf[1] = buf[1] >> 4;
	}
	else
	{
		buf[2] = 64;
		buf[1] = 0;
	}
	if (i >= 1)
	{
		buf[1] = buf[1] + ((buf[0] << 4) & 0x30);
		buf[0] = buf[0] >> 2;

	}
}

void ind_to_char(unsigned char *buf)
{
	buf[0] = (buf[0] << 2) + (buf[1] >> 4);
	buf[1] = (buf[1] << 4) + (buf[2] >> 2);
	buf[2] = (buf[2] << 6) + buf[3];

}


void ind_to_b64(unsigned char *buf, unsigned int r)
{
	for (unsigned int i = 0; i <= 3; i++)
	{
		if (buf[i] < 26)
			buf[i] = buf[i] + 'A';
		else if (buf[i] < 52)
			buf[i] = buf[i] + 'a' - 26;
		else if (buf[i] < 62)
			buf[i] = buf[i] + '0' - 52;
		else if (buf[i] == 62)
			buf[i] = '+';
		else if (buf[i] == 63)
			buf[i] = '/';
		else if (buf[i] == 64)
			buf[i] = '=';
	}
}

void b64_to_ind(unsigned char *c)
{
	if ((*c >= 'A') && (*c <= 'Z'))
		*c = *c - 'A';
	else if ((*c >= 'a') && (*c <= 'z'))
		*c = *c - 'a' + 26;
	else if ((*c >= '0') && (*c <= '9'))
		*c = *c - '0' + 52;
	else if (*c == '+')
		*c = 62;
	else if (*c == '/')
		*c = 63;
	else if (*c == '=')
		*c = '\0';
	else *c = 65;
}

void encode(FILE *f1, FILE *f2, unsigned int space)
{
	unsigned char buf[8] = { 0 };
	size_t r = 0;
	unsigned int k = 0;
	while ((r = fread(buf, sizeof(char), 3, f1)) != 0)
	{
		char_to_ind(buf, r);
		ind_to_b64(buf, r);
		if (space == 0)
			fwrite(buf, sizeof(char), 4, f2);
		else
		for (unsigned int i = 0; i < 4; i++)
		{
			fwrite(buf + i, sizeof(char), 1, f2);
			k++;
			if ((k % space) == 0)
				fwrite("\n", sizeof(unsigned char), 1, f2);
		}
	}

}
unsigned int decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space)
{
	unsigned char buf[8] = { 0 };
	unsigned int k = 0;
	unsigned int i = 0;

	while (i == 0)
	{
		while (i < 4)
		{
			if (fread(buf + i, sizeof(unsigned char), 1, f1) != 0)
			{
				b64_to_ind(&buf[i]);
				if (buf[i] < 65)
					i++;
				else if (ign)
					continue;
				else return 1;

			}
			else return 0;
		}
		i = 0;
		ind_to_char(buf);
		if (space == 0)
			fwrite(buf, sizeof(char), 3, f2);
		else
		for (unsigned int j = 0; j < 3; j++)
		{
			fwrite(buf + j, sizeof(char), 1, f2);
			k++;
			if ((k % space) == 0)
				fwrite("\n", sizeof(unsigned char), 1, f2);
		}
	}
	fwrite("\0", sizeof(unsigned char), 1, f2);
	fwrite("\0", sizeof(unsigned char), 1, f2);
	return 0;
}

void help(void)
{
	printf("Usage:\n"
		"%%mode%% %%option%% %%input_name%% %%output_name%%\n"
		"Modes:\n"
		"-d      // decode\n"
		"-i -d   // decode ignoring wrong symbols\n"
		"-e      // encode\n"
		"Options:\n"
		"-f N    // line break after N>0 symbols\n"
		"Example:\n"
		"-i -d -f 80 input.txt output.txt");
}