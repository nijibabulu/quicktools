#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

#define LINE_LEN 60

void usage (char * progname)
{
    fprintf(stderr, "%s fasta-file\n", progname);
    fprintf(stderr, "Outputs the melting temperature of the sequences in a fasta file\n");
    fprintf(stderr, "Uses the 4*GC + 2*AT method, unless the sequence is 15 bases or more\n");
    fprintf(stderr, "in which case it uses the 64.9 + 41*(GC-16.4)/L method.\n");
    exit(1);
}

void output_tm(char *name, float a, float c, float g, float t, int len) 
{
  float tm;
  if(len > 14) 
    tm = 64.9 + 41*((g+c-16.4)/len);
  else
    tm = 4*(g+c) + 2*(t+a);
  printf("%s: %f\n",name,tm);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char name[2048], ch, *p;
    int len;
    float a,c,g,t;

    if(argc != 2) 
        usage(argv[0]);

    fasta = fopen(argv[1], "r");
    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        exit(1);
    }
    len = a = g = c = t = 0;
    while((ch = fgetc(fasta)) != EOF) {
        switch(ch) {
        case '>':
            if(a || g || c || t)
              output_tm(name,a,c,g,t,len);
            fgets(name, 2048, fasta);
            for(p = name; !isspace(*p) && *p != '\0'; p++);
            *p = '\0';
            len = a = g = c = t = 0;
            break;
        case 'a': case 'A': a++; len++; break;
        case 'c': case 'C': c++; len++; break;
        case 'g': case 'G': g++; len++; break;
        case 't': case 'T': t++; len++; break;
        default: break;
        }
    }
    if(a || g || c || t)
      output_tm(name,a,c,g,t,len);
    fclose(fasta);
    return 0;
}
