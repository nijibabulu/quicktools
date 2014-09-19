#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void usage() 
{
  fprintf(stderr, 
  "Usage: make_fasta [options] sequence\n"
  "\n"
  "Quickly create a fasta file from a sequence and write it to standard out.\n"
  "Options:\n"
  "  -u     make all characters uppercase\n"
  "  -l     make all characters lowercase\n"
  "  -i ID  set the id of the sequence to ID\n"
  );
  exit(1);
}

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  FILE * out = stdout;
  char *id = "seq",c;
  int u = 0, l = 0, len,i;

  while((c = getopt(argc,argv,"i:lu")) != -1) {
    switch(c) {
      case 'u': u = 1; break;
      case 'l': l = 1; break;
      case 'i':
        id = optarg;
        break;
      case 'h': default:
        usage(); break;
    }
  }
  if(argc-optind != 1) usage();
  argv += optind;
  argc -= optind;
  if(u == 1 && l == 1) {
    fprintf(stderr, "-u and -l are mutually exlusive\n");
    usage();
  }
  len = strlen(argv[0]);
  fprintf(out, ">%s\n", id);
  for(i = 0; i < len; i++) {
    c = argv[0][i];
    if(u == 1) 
      c = toupper(c);
    if(l == 1)
      c = tolower(c);
    fputc(c, out);
  }
  fputc('\n', out);
  return 0;
}
