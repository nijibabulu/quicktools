#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<unistd.h>

void usage (char * progname)
{
    fprintf(stderr, "%s [options] num file-name\n", progname);
    fprintf(stderr, " -h    display help\n");
    exit(1);
}

int cmpint (const void *a, const void *b) { return (*(int *)a - *(int *)b);}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[2048], c;
    long int *samples,tmp,max,nseq,r,i,nsamples,cur_sample,n;

    while((c = getopt(argc, argv, "h")) != -1) {
      switch(c) {
      case 'h':
        usage(argv[0]);
        break;
      }
    }

    if(argc - optind < 2) 
        usage(argv[0]);
    argv += optind;
    argc -= optind;

    nsamples = strtol(argv[0],NULL,0);
    fasta = fopen(argv[1], "r");

    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        exit(1);
    }
    nseq = 0;
    while((c = fgetc(fasta)) != EOF) {
      if(c=='>')
        nseq++;
    }
    fclose(fasta);

    samples = malloc(sizeof(long int)*nseq);
    for(i = 0; i < nseq; i++) 
      samples[i] = i;

    srand(time(NULL));
    for(max = nseq-1; max >= 1; max--) {
      r = rand() % max;
      tmp = samples[max]; samples[max] = samples[r]; samples[r] = tmp;
    }

    qsort(samples,nsamples,sizeof(long int),cmpint);


    fasta = fopen(argv[1], "r");
    for(cur_sample = 0, n = 0; (c = fgetc(fasta)) != EOF;) {
      if(c == '>') 
        n++;
      if(samples[cur_sample] == n-1)
        putchar(c);
      if(samples[cur_sample] == n-2)
        cur_sample++;
    }
    fclose(fasta);
    return 0;
}
