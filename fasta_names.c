#include <stdio.h>
#include <stdlib.h>

void usage() 
{
  fprintf(stderr, 
  "Usage: fasta_names file\n"
  "\n"
  "Return all the fasta names from the header lines of a multi-fasta file.\n"
  "Useful for command-line extraction of fasta-file names.\n");
  exit(1);
}

int
main(int argc, char *argv[])
{
  char buf[4096];
  FILE *f;
  int i;
  
  if(argc != 2) usage();
  f = fopen(argv[1],"r");
  if(f == NULL) {
    fprintf(stderr, "Could not open %s for reading.", argv[1]);
    exit(1);
  }
  while((fgets(buf,4096,f))) {
    if(buf[0] == '>') {
      for(i = 0; !isspace(buf[i]); i++);
      buf[i] = '\0';
      printf("%s\n", buf + 1);
    }
  }
  return 0;
  
}
