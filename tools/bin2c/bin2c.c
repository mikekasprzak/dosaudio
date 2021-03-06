// Convert a binary file to a format usable as data inside C source code.
// -jcw, 2015-11-09

#include <stdio.h>

int main (int argc, const char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: bin2h infile >outfile\n");
        return 1;
    }

    FILE* inf = fopen(argv[1], "rb");
    if (inf == 0) {
        perror(argv[1]);
        return 2;
    }

    while (1) {
        int c = fgetc(inf);
        if (c < 0)
            break;
        printf("%d,", c);
    }

    return 0;
}
