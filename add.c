#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void usage (char * progname)
{
    fprintf(stderr, "%s [options] [file-name ...]\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -p num  output with given number of decimal places\n");
    fprintf(stderr, " -f    output a floating-point number\n");
    fprintf(stderr, " -h    display this help and exit\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * in;
    char buf[2048], c;
    long double cur,total;
    int use_float,precision;
    int status;

    use_float = 0;
    precision = 3;
    while((c = getopt(argc, argv, "fp:h")) != -1) {
      switch(c) {
      case 'f':
        use_float = 1;
        break;
      case 'p':
        status = sscanf(optarg,"%d",&precision);
        if(status != 1) {
          fprintf(stderr, "Invalid precision %s\n", optarg);
          exit(1);
        }
        break;
      case 'h':
      default:
        usage(argv[0]);
        break;
      }
    }

    /*if(argc-optind != 1) 
      usage(argv[0]);*/

    argv += optind;
    argc -= optind;

    if(argc == 0) 
      in = stdin;
    else {
      in = fopen(argv[0], "r");
      if(in == NULL) {
          fprintf(stderr, "Could not open %s for reading.\n", argv[0]);
          exit(1);
      }
    }
    total = 0.;
    while(1) {
      if((status = fscanf(in, "%Le", &cur)) == 1) 
        total += cur;
      else
        break;
    }
    if(status != EOF) {
      fprintf(stderr, "WARNING: error reading numbers\n");
    }
    if(use_float) 
      printf("%.*Le\n", precision, total);
    else
      printf("%lu\n", (long unsigned int)total);
    return 0;
}
