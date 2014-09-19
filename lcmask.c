#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

void usage (char * progname)
{
    fprintf(stderr, "%s file-name\n", progname);
    fprintf(stderr, "Converts all lowercase DNA characters into 'N's\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[256], c;

    if(argc != 2) 
        usage(argv[0]);

    fasta = fopen(argv[1], "r");
    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        exit(1);
    }
    while((c = fgetc(fasta)) != EOF) {
        switch(c) {
        case '>':
            fgets(buf, 256, fasta);
            printf(">%s", buf);
            break;
        case 'a': case 'c': case 'g': case 't':
        case 'm': case 'r': case 'w': case 's': case 'y': case 'k':
        case 'v': case 'h': case 'd': case 'b':
            printf("N");
            break;
        default:
            printf("%c", c);
            break;
        }
    }
    fclose(fasta);
    return 0;
}
