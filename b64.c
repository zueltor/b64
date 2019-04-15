#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void char_to_ind(unsigned char *buf, unsigned int i);
void ind_to_char(unsigned char *buf, unsigned int r);
void ind_to_b64(unsigned char *buf, unsigned int r);
void b64_to_ind(unsigned char *buf, unsigned int r);
void encode(FILE *f1, FILE *f2, unsigned int space);
void decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space);
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
		printf("space = %d ", space);
		encode(f1, f2, space);
	} else	
		if (!strcmp(argv[i], "-d"))
		{
			if (!strcmp(argv[1], "-i") && i == 2)
				ign = 1;
			else if (i != 1)
				err = 2;
			printf("ign = %d space = %d ", ign, space);
			decode(f1, f2, ign, space);
		}
	
	else
		err = 3;
	printf("err = %d", err);
	/*while ((r = fread(buf, sizeof(char), 3, f1)) != 0)
	{
		printf("read= %c %c %c %d\n", buf[0], buf[1], buf[2], r);
		char_to_ind(buf, r);
		ind_to_b64(buf, r);
		fwrite(buf, sizeof(char), 4, f2);
		printf("%c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);
	}*/

	/*while ((r = fread(buf, sizeof(unsigned char), 4, f1)) != 0)
	{
		b64_to_ind(buf, r);
		ind_to_char(buf, r);
		//printf("%d %d %d %d\n", buf[0], buf[1], buf[2], buf[3]);
		fwrite(buf, sizeof(char), 3, f2);
		printf("%c %c %c\n", buf[0], buf[1], buf[2]);
	}*/

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

void ind_to_char(unsigned char *buf, unsigned int r)
{
	if (r != 4)
	{
		for (unsigned int i = 0; i < 3; i++)
			buf[i] = '\0';
		return;
	}

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

void b64_to_ind(unsigned char *buf, unsigned int r)
{
	for (unsigned int i = 0; i <= 4; i++)
	{
		if ((buf[i] >= 'A') && (buf[i] <= 'Z'))
			buf[i] = buf[i] - 'A';
		else if ((buf[i] >= 'a') && (buf[i] <= 'z'))
			buf[i] = buf[i] - 'a' + 26;
		else if ((buf[i] >= '0') && (buf[i] <= '9'))
			buf[i] = buf[i] - '0' + 52;
		else if (buf[i] == '+')
			buf[i] = 62;
		else if (buf[i] == '/')
			buf[i] = 63;
		else
			buf[i] = '\0';

	}
}

void encode(FILE *f1, FILE *f2, unsigned int space)
{
	unsigned char buf[8] = { 0 };
	size_t r = 0;
	unsigned int k = 0;
	while ((r = fread(buf, sizeof(char), 3, f1)) != 0)
	{
		//printf("read= %c %c %c %d\n", buf[0], buf[1], buf[2], r);
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
		//fwrite(buf, sizeof(char), 4, f2);
		printf("%c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);
	}

}
void decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space)
{
	unsigned char buf[8] = { 0 };
	size_t r = 0;
	unsigned int k = 0;
	while ((r = fread(buf, sizeof(unsigned char), 4, f1)) != 0)
	{

		b64_to_ind(buf, r);
		ind_to_char(buf, r);
		if (space == 0)
			fwrite(buf, sizeof(char), 3, f2);
		else
			for (unsigned int i = 0; i < 3; i++)
			{
				fwrite(buf + i, sizeof(char), 1, f2);
				k++;
				if ((k % space) == 0)
					fwrite("\n", sizeof(unsigned char), 1, f2);
			}
		
		//fwrite(buf, sizeof(char), 3, f2);
		printf("%c %c %c\n", buf[0], buf[1], buf[2]);
	}

}