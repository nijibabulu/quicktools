#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<unistd.h>

double dA = 313.21, dC = 289.18, dG = 329.21, dT = 304.2;
double A = 329.21, C = 305.18, G =  345.21, U = 306.17;

#define RNA 0
#define SSDNA 1
#define DSDNA 2

void usage (char * progname)
{
    fprintf(stderr, "%s [options] file-name [ file-name ... ]\n", progname);
    fprintf(stderr, "  -r       sequence is RNA (default - Ts are converted to Us)\n");
    fprintf(stderr, "  -d       sequence is single-stranded DNA\n");
    fprintf(stderr, "  -D       sequence is double-stranded DNA\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[2048], c;
    int type, i;
    double weight, cur;
    
    type = RNA;

    while((c = getopt(argc, argv, "dDr")) != -1) {
      switch(c) {
      case 'r': type = RNA; break;
      case 'd': type = SSDNA; break;
      case 'D': type = DSDNA; break;
      case 'h': usage(argv[0]); break;
      }
    }

    if(argc-optind < 1) 
      usage(argv[0]);

    argv += optind;
    argc -= optind;

    for(i = 0; i < argc; i++) {
      fasta = fopen(argv[i], "r");
      if(fasta == NULL) {
          fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
          exit(1);
      }
      weight = 0.0;
      while((c = fgetc(fasta)) != EOF) {
          switch(c) {
          case '>':
              if(weight != 0.0) 
                printf("%0.2f %s", weight, buf);
              weight = 0.0;
              fgets(buf, 2048, fasta);
              break;
          case ' ': case '\n':
              break;
          default:
              if(type == RNA) {
                switch(toupper(c)) {
                  case 'A': weight += A; break;
                  case 'C': weight += C; break;
                  case 'G': weight += G; break;
                  case 'T': case 'U': weight += U; break;
                }
              }
              else {
                switch(toupper(c)) {
                  case 'A': cur = dA; break;
                  case 'C': cur = dC; break;
                  case 'G': cur = dG; break;
                  case 'T': cur = dT; break;
                }
                if(type == DSDNA)
                  cur *= 2;
                weight += cur;
              }
              break;
          }
      }
      printf("%0.2f %s", weight, buf);
    }
    fclose(fasta);
    return 0;
}
