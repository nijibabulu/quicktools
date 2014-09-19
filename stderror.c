#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
void usage (char * progname)
{
    fprintf(stderr, "%s [file-name ...]\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -p num  output with given number of decimal places\n");
    fprintf(stderr, " -h      display this help and exit\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * in;
    char buf[2048], c;
    double cur,total,total2,mean,num;
    int status,precision;

    precision = 3;
    while((c = getopt(argc, argv, "p:h")) != -1) {
      switch(c) {
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
      if((status = fscanf(in, "%lf", &cur)) == 1) {
        total += cur;
        total2 += cur*cur;
      }
      else
        break;
      num++;
    }
    if(status != EOF) {
      fprintf(stderr, "WARNING: error reading numbers\n");
    }
    mean = total/num;
    printf("%.*lf\n", precision, sqrt(total2/num - mean*mean)/sqrt(num));
    return 0;
}
