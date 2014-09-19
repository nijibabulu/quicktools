#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>

#define LINE_LEN 60

void usage(char * progname)
{
  fprintf(stderr, "%s [options] [fasta-file]\n", progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Reverse compliment sequences in a fasta file.\n");
  fprintf(stderr, "If no file is given, stdin is used\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "   -h    print this help message and exit.\n");
  exit(1);
}

/*   **Modified from squid
 *
 * Function: revcomp()
 *
 * Purpose:  Reverse complement seq; store in comp.
 *           Can revcomp "in place" (revcomp(seq, seq)).
 *
 * Args:     comp  - destination for reverse complement of seq
 *           seq   - sequence to reverse complement
 *
 * Returns:  NULL on failure; or a (useless) pointer to comp.
 */
char *
revcomp(char *comp, char *seq)
{
  char *s,*tmp;
  char  c;
  int len,i;

  if (comp == NULL) return NULL;
  if (seq == NULL)  return NULL;

  len = strlen(seq);
  if(seq == comp) {
    for(i = 0; i < len/2; i++) {
      seq[i] ^= seq[len-1-i];
      seq[len-1-i] ^= seq[i];
      seq[i] ^= seq[len-1-i];
    }
  } 
  else {
    for(i = 0; i < len; i++) 
      comp[i] = seq[len-1-i];
    comp[i] = '\0';
  }
  for (s = comp; *s != '\0'; s++)
    {
      c = *s;
      c = toupper(c);
      switch (c) {
      case 'A': c = 'T'; break;
      case 'C': c = 'G'; break;
      case 'G': c = 'C'; break;
      case 'T': c = 'A'; break;
      case 'U': c = 'A'; break;
      case 'R': c = 'Y'; break;
      case 'Y': c = 'R'; break;
      case 'M': c = 'K'; break;
      case 'K': c = 'M'; break;
      case 'S': c = 'S'; break;
      case 'W': c = 'W'; break;
      case 'H': c = 'D'; break;
      case 'D': c = 'H'; break;
      case 'B': c = 'V'; break;
      case 'V': c = 'B'; break;
      default:  break;		/* anything else? leave it; it's prob a gap or an X */
      }
      if (islower((int) *s)) c = (char) tolower((int) c);
      *s = c;
    }
  return comp;
}

#define START_LENGTH 1000

int
main(int argc, char *argv[])
{
  extern char * optarg;
  extern int optind;
  FILE *fasta;
  char header[2048], c;
  char *str;
  long len,pos,i;
  int cur_file;

  while((c = getopt(argc,argv,"h")) != -1) {
    switch(c) {
    case 'h':
    default:
      usage(argv[0]);
      break;
    }
  }

  argv += optind;
  argc -= optind;

  str = malloc(START_LENGTH);
  len = START_LENGTH;
  pos = 0;
  cur_file = 0;

  while(1) {
    if(argc > 0) {
      if(cur_file >= argc)
        break;
      fasta = fopen(argv[cur_file++], "r");

      if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[0]);
        exit(1);
      }
    }
    else 
      fasta = stdin;

    do {
      c = fgetc(fasta);
      switch(c) {
      case '>':
      case EOF:
        if(pos > 0) {
          revcomp(str,str);
          for(i = 0; i < pos; i++) {
            if(i%LINE_LEN==0 && i) 
              putchar('\n');
            putchar(str[i]);
          }
          if(i%LINE_LEN != 0)
            putchar('\n');
          pos = 0;
        }
        if(c == EOF)
          break;
        fgets(header, 2048, fasta);
        printf(">%s", header);
        break;
      case ' ': case '\n': case '\r':
        break;
      default:
        if(pos == len) {
          len *= 2;
          str = realloc(str, len);
          if(str == NULL) {
            fprintf(stderr, "Error allocating memory.  Quitting.\n");
            exit(1);
          }
        }
        str[pos++] = c;
      }
    } while(c != EOF);
    if(fasta == stdin) 
      break;
  }
  return 0;
}
