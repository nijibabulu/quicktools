#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<libgen.h>

#define LINE_LEN 60
#define PLUS '+'
#define MINUS '-'

#define USAGE_SUBSTR "fasta_substr [options] fasta-file start [length]"
#define USAGE_RANGE "fasta_range [options] fasta-file start [end]"

void usage (const char * usage_line)
{
  /*fprintf(stderr, "%s [options] fasta-file start [length]\n", progname);*/
  fprintf(stderr, "%s\n", usage_line);
  fprintf(stderr, "\n");
  fprintf(stderr, "Fetch a substring of the first sequence in a fasta file.\n");
  fprintf(stderr, "If length is not given, fetch the sequence starting at\n");
  fprintf(stderr, "the given start point to the end of the first sequence.\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h     print this help message and exit\n");
  fprintf(stderr, "  -n     name of the sequence [name_start:length]\n");

  exit(1);
}

int main(int argc, char *argv[]) 
{
  extern char *optarg;
  extern int optind;
  FILE * fasta;
  char header[2048], c,d;
  int pos, start, length, i, j, complen;
  char *str,*final,*name,*usage_line;

  name = NULL;
  if(strcmp(basename(argv[0]), "fasta_substr") == 0) {
    complen = 0; usage_line = USAGE_SUBSTR; 
  } else if(strcmp(basename(argv[0]), "fasta_range") == 0) {
    complen = 1; usage_line = USAGE_RANGE; 
  }

  while((c = getopt(argc,argv,"n:h")) != -1) {
    switch(c){
    case 'n': name = optarg; break;
    case 'h':
    default:
      usage(usage_line);
      break;
    }
  }

  if(argc-optind != 2 && argc-optind != 3)
      usage(usage_line);

  argv += optind;
  argc -= optind;

  start = atoi(argv[1]);
  length = 0;
  if(argc == 3)
    length = atoi(argv[2]);
  if(complen)
    length -= start;

  fasta = fopen(argv[0], "r");
  if(fasta == NULL) {
      fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
      exit(1);
  }
  pos = 0;
  i = 0;
  while((c = fgetc(fasta)) != EOF) {
    if((length && (pos >= start+length)) || (pos && c == '>'))
      break;
    switch(c) {
    case '>':
      for(d = c; !isspace(d) && d != EOF; d = fgetc(fasta)) 
        putchar(d);
      printf("_%d:", start);
      if(length)
        printf("%d",length);
      for(; d != '\n' && d != '\r'; d = fgetc(fasta))
        putchar(d);
      putchar(d);
      break;
    case ' ': case '\n':
      break;
    default:
      if(pos >= start)  {
        if(i%LINE_LEN==0 && i)
          putchar('\n');
        putchar(c);
        i++;
      }
      pos++;
      break;
    }
  }
  if(i%LINE_LEN == 0 || i) putchar('\n');
  fclose(fasta);
  return 0;
}
