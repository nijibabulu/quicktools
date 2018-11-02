#include<ctype.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<time.h>
#ifdef __APPLE__
#include<mach/mach_time.h>
#endif


enum alphabet_e {
  DNA=0,
  RNA,
  PROTEIN,
  NALPH
};
char alphabets[NALPH][25] = {
  "ACGT",
  "ACGU",
  "GPAVLIMCFYWHKRQNEDST"
};

void
usage () 
{
  fprintf(stderr, "Usage: randseq [options] [count]\n");
  fprintf(stderr, "Create random sequences.  Default length is 40; default count is 1\n");

  fprintf(stderr, "\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -o OUT   leave result in OUT\n");
  fprintf(stderr, "  -p       use a protein alphabet\n");
  fprintf(stderr, "  -r       use an RNA alphabet\n");
  fprintf(stderr, "  -l       specify the length specifying two numbers\n");
  fprintf(stderr, "           will generate sequences of random length \n");
  fprintf(stderr, "           within the given range (default: 40)\n");
  fprintf(stderr, "  -f       use FASTA format\n");
  fprintf(stderr, "  -n ARG   use ARG as name for sequences\n");
  fprintf(stderr, "  -h       display this help message and exit\n");

  exit(0);
}


void
do_seed()
{
#ifdef __APPLE__
  srand(mach_absolute_time());
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  srand(ts.tv_nsec);
#endif
}
int
main(int argc, char *argv[])
{
  int minlen=40,maxlen=40,count=1,alphabet=DNA,use_fasta=0;
  int alph_size,i,j,optlen,seqlen;
  char c,*name="seq",*outfilename=NULL;
  FILE * out;
  extern int optind;
  extern char * optarg;

  do_seed();
  out = stdout;
  
  while((c = getopt(argc,argv,"o:l:n:hprf")) != -1) {
    switch(c) {
    case 'l':
      minlen = maxlen = strtol(optarg,NULL,0);
      optlen = strlen(optarg);
      for(i = 0; i < optlen && isdigit(optarg[i]); i++);
      for(; i < optlen && !isdigit(optarg[i]); i++);
      if(i < optlen)
        maxlen = strtol(optarg+i,NULL,0);
      break;
    case 'n':
      name=optarg;
      break;
    case 'o':
      out = fopen(optarg, "w");
      break;
    case 'p':
      alphabet=PROTEIN;
      break;
    case 'r':
      alphabet=RNA;
      break;
    case 'f':
      use_fasta=1;
      break;
    case 'h':
      usage();
      break;
    }
  }


  argv += optind;
  argc -= optind;

  if(argc > 0) 
    count = strtol(argv[0],NULL,0);

  alph_size=strlen(alphabets[alphabet]);
  for(i = 0; i < count; i++) {
    if(use_fasta == 1) {
      if(count > 1) {
	fprintf(out,">%s%d\n",name,i);
      } else {
	fprintf(out,">%s\n",name);
      }
    }
    if(minlen == maxlen) {
      seqlen = maxlen;
    } else {
      seqlen = (int)rand()%(maxlen-minlen+1) + minlen;
    }
    for(j = 0; j < seqlen; j++)
      fputc(alphabets[alphabet][(int) (rand() % alph_size)], out);
    fputc('\n',out);
  }
  return 0;
}

