#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<unistd.h>

void usage (char * progname)
{
    fprintf(stderr, "%s file-name\n", progname);
    fprintf(stderr, "Converts all lowercase DNA letters to uppercase\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -N     convert lowercase letters to N\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[256], c, u, N;
    int i;

    N = 0;
    while((c = getopt(argc, argv, "Nh")) != -1) {
      switch(c) {
      case 'N':
        N = 1;
        break;
      case 'h':
        usage(argv[0]);
        break;
      }
    }

    if(argc - optind < 1) 
        usage(argv[0]);
    argv += optind;
    argc -= optind;

    for(i = 0; i < argc; i++) {
      fasta = fopen(argv[i], "r");
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
              if(N) 
                u = 'N';
              else
                u = toupper(c);
              printf("%c", u);
              break;
          default:
              printf("%c", c);
              break;
          }
      }
      fclose(fasta);
    }
    return 0;
}
