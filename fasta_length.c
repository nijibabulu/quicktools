#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

void usage (char * progname)
{
    fprintf(stderr, "%s file-name\n", progname);
    fprintf(stderr, "Outputs the length of fasta file.\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[2048], c;
    int total,saw_multiple;

    if(argc != 2) 
        usage(argv[0]);

    fasta = fopen(argv[1], "r");
    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        exit(1);
    }
    total = 0;
    saw_multiple = 0;
    while((c = fgetc(fasta)) != EOF) {
        switch(c) {
        case '>':
            if(total) {
              printf("%d %s", total, buf);
              saw_multiple=1;
              total = 0;
            }
            fgets(buf, 2048, fasta);
            break;
        /*
        case 'A': case 'C': case 'G': case 'T':
        case 'M': case 'R': case 'W': case 'S': case 'Y': case 'K':
        case 'V': case 'H': case 'D': case 'B':
        case 'a': case 'c': case 'g': case 't':
        case 'm': case 'r': case 'w': case 's': case 'y': case 'k':
        case 'v': case 'h': case 'd': case 'b':
        case 'N': case 'n':
            total++;
            break;
        */
        case ' ': case '\n':
            break;
        default:
            total++;
            break;
        }
    }
    if(saw_multiple)
      printf("%d %s\n", total, buf);
    else
      printf("%d\n", total);
    fclose(fasta);
    return 0;
}
