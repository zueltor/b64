#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void char_to_ind(unsigned char *buf, unsigned int i);
void ind_to_char(unsigned char *buf);
void ind_to_b64(unsigned char *buf, unsigned int r);
void b64_to_ind(unsigned char *c);
void encode(FILE *f1, FILE *f2, unsigned int space);
unsigned int decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space);
int main(int argc, char *argv[])
{

	if ((argc > 7) || (argc < 4))
	{
		printf("bad input");
		return 1;
	}
	FILE *f1 = fopen(argv[argc-2], "rb");
	FILE *f2 = fopen(argv[argc-1], "wb");
	unsigned int i,
		err,
		ign;
	int space; 
	if (f1 == NULL)
	{
		printf("error %s", argv[argc-2]);
		return 1;
	}
	if (f2 == NULL)
	{
		printf("error %s", argv[argc-1]);
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
	} else	
		if (!strcmp(argv[i], "-d"))
		{
			if (!strcmp(argv[1], "-i") && i == 2)
				ign = 1;
			else if (i != 1)
				err = 1;
			if (decode(f1, f2, ign, space))
				printf("file has wrong symbol(s)");
		}
	
	else
		err = 1;
	if (err)
		printf("Usage:\n"
			"%%mode%% %%option%% %%input_name%% %%output_name%%\n"
			"Modes:\n"
			"-d // decode\n"
			"-i -d // decode ignoring wrong symbols\n"
			"-e // encode\n"
			"Options:\n"
			"-f N // line break after N symbols\n"
			"Example:\n"
			"-i -d -f 80 input.txt output.txt");

	fclose(f1);
	fclose(f2);
	return 0;

}


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
	return 0;
}
