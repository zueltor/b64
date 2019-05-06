#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int b64(char **argv, unsigned int n) {
    FILE *f1 = fopen(argv[n - 2], "rb");
    FILE *f2 = fopen(argv[n - 1], "wb");
    unsigned int i,
            err,
            ign;
    int space;
    if (f1 == NULL) {
        printf("error %s", argv[n - 2]);
        return 1;
    }
    if (f2 == NULL) {
        printf("error %s", argv[n - 1]);
        return 1;
    }

    err = 0;
    i = n - 3;
    space = 0;
    ign = 0;
    if (space = atoi(argv[i])) {
        if (!strcmp(argv[i - 1], "-f") && (space > 0)) {
            i -= 2;
        } else
            err = 1;
    }

    if (!strcmp(argv[i], "-e") && i == 1) {
        encode(f1, f2, space);
    } else if (!strcmp(argv[i], "-d")) {
        if (!strcmp(argv[1], "-i") && i == 2)
            ign = 1;
        else if (i != 1)
            err = 1;
        if (decode(f1, f2, ign, space))
            printf("file has wrong symbol(s)");
    } else
        err = 1;
    if (err)
        print_help();

    fclose(f1);
    fclose(f2);
    return 0;
}

void chars_to_b64(unsigned char *buf_in, unsigned char *buf_out, unsigned int r) {
    int i = 0,
            j = 0,
            k = r;
    while (k >= 3) {
        buf_out[i] = buf_in[j] >> 2;
        buf_out[++i] = ((buf_in[j] << 4) & 0x30) + (buf_in[++j] >> 4);
        buf_out[++i] = ((buf_in[j] << 2) & 0x3c) + (buf_in[++j] >> 6);
        buf_out[++i] = buf_in[j] & 0x3F;
        i++;
        j++;
        k -= 3;
    }
    if (k >= 1) {
        buf_out[i] = buf_in[j] >> 2;
        buf_out[++i] = ((buf_in[j] << 4) & 0x30) + (buf_in[++j] >> 4);
        buf_out[++i] = '=';
        buf_out[i + 1] = '=';
    }
    if (k == 2) {
        buf_out[i++] = ((buf_in[j] << 2) & 0x3c) + (buf_in[++j] >> 6);
    }

    for (j = 0; j < i; j++) {
        if (buf_out[j] < 26)
            buf_out[j] = buf_out[j] + 'A';
        else if (buf_out[j] < 52)
            buf_out[j] = buf_out[j] + 'a' - 26;
        else if (buf_out[j] < 62)
            buf_out[j] = buf_out[j] + '0' - 52;
        else if (buf_out[j] == 62)
            buf_out[j] = '+';
        else if (buf_out[j] == 63)
            buf_out[j] = '/';
        else if (buf_out[j] == 64)
            buf_out[j] = '=';
    }

}

void f_out(unsigned char *buf, unsigned int space, unsigned int n, FILE *f2, unsigned int mode) {
    unsigned int i = 0;
    if (mode == 1)
        n = (4 - (n % 3) % 4) % 4 + n * 4 / 3;
    else n = n * 3 / 4;
    if (space == 0)
        fwrite(buf, sizeof(char), n, f2);
    else
        while (n) {
            if (n > space) {
                fwrite(buf + i, sizeof(char), space, f2);
                i += space;
                n -= space;
                fwrite("\n", sizeof(char), 1, f2);
            } else {
                fwrite(buf + i, sizeof(char), n, f2);
                n = 0;
            }
        }
}

void encode(FILE *f1, FILE *f2, unsigned int space) {
    unsigned char buf_in[BUF_SIZE * 3] = {0};
    unsigned char buf_out[BUF_SIZE * 4] = {0};
    unsigned int r = 0,
            n = BUF_SIZE * 3;


    while ((r = fread(buf_in, sizeof(char), n, f1)) != 0) {
        chars_to_b64(buf_in, buf_out, r);
        if (r == n) {
            f_out(buf_out, space, n, f2, 1);
            memset(buf_in, 0, n * sizeof(char));
        } else
            f_out(buf_out, space, r, f2, 1);
    }
}

int b64_to_chars(unsigned char *buf_in, unsigned char *buf_out, unsigned int ign, unsigned int n,
                 unsigned int *new_length) {
    unsigned int i = 0, j = 0;
    while (i < n) {
        if ((buf_in[i] >= 'A') && (buf_in[i] <= 'Z'))
            buf_in[j] = buf_in[i] - 'A';
        else if ((buf_in[i] >= 'a') && (buf_in[i] <= 'z'))
            buf_in[j] = buf_in[i] - 'a' + 26;
        else if ((buf_in[i] >= '0') && (buf_in[i] <= '9'))
            buf_in[j] = buf_in[i] - '0' + 52;
        else if (buf_in[i] == '+')
            buf_in[j] = 62;
        else if (buf_in[i] == '/')
            buf_in[j] = 63;
        else if (buf_in[i] == '=')
            buf_in[j] = '\0';
        else if (ign) {
            i++;
            continue;
        } else return 1;
        j++;
        i++;
    }
    i = 0;
    n = j;
    j = 0;
    while (n > 0) {
        buf_out[i++] = (buf_in[j] << 2) + (buf_in[++j] >> 4);
        buf_out[i++] = (buf_in[j] << 4) + (buf_in[++j] >> 2);
        buf_out[i++] = (buf_in[j++] << 6) + buf_in[j++];
        n -= 4;
    }
    if (buf_in[j - 2] == '\0')
        *new_length = j - 2;
    else if (buf_in[j - 1] == '\0')
        *new_length = j - 1;
    else *new_length = j;

    return 0;
}


unsigned int decode(FILE *f1, FILE *f2, unsigned int ign, unsigned int space) {
    unsigned char buf[8] = {0};
    unsigned char buf_in[BUF_SIZE * 4];
    unsigned char buf_out[BUF_SIZE * 3];
    unsigned int k = 0,
            i = 0,
            j = 0,
            r = 0,
            n = BUF_SIZE * 4;

    while ((r = fread(buf_in, sizeof(char), n, f1)) != 0) {
        if (b64_to_chars(buf_in, buf_out, ign, r, &k))
            return 1;
        if (r == n) {
            f_out(buf_out, space, k, f2, 0);
            memset(buf_in, 0, n * sizeof(char));
        } else
            f_out(buf_out, space, k, f2, 0);

    }

}

void print_help(void) {
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
}