#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<unistd.h>

void usage (char * progname)
{
    fprintf(stderr, "%s [options] file-name [ file-name ... ]\n", progname);
    fprintf(stderr, "  -l       output total residues in file (default)\n");
    fprintf(stderr, "  -n       output number of sequences in file\n");
    fprintf(stderr, "  -m       output mean length of sequences in file\n");
    fprintf(stderr, "  -s       only output the summary of all sequences\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[2048], c;
    int nseqs,totbases,nbases,saw_multiple,i;
    int output_num,output_length,output_mean_length,output_summary_only;

    output_num = output_length = output_mean_length = output_summary_only = 0;

    while((c = getopt(argc, argv, "nlmsh")) != -1) {
      switch(c) {
      case 's':
        output_summary_only = 1;
        break;
      case 'n':
        output_num = 1;
        break;
      case 'l':
        output_length = 1;
        break;
      case 'm':
        output_mean_length = 1;
        break;
      case 'h':
        usage(argv[0]);
        break;
      }
    }

    if((output_num + output_length + output_mean_length) == 0) 
      output_length = 1;

    if(argc-optind < 1) 
      usage(argv[0]);

    argv += optind;
    argc -= optind;

    if(output_num + output_length + output_mean_length > 1 && output_summary_only){
      if(output_num)
        printf("nseqs\t");
      if(output_length)
        printf("total_len\t");
      if(output_mean_length)
        printf("mean_length\t");
      printf("\n");
    }

    for(i = 0; i < argc; i++) {
      fasta = fopen(argv[i], "r");
      if(fasta == NULL) {
          perror("Error: ");
          fprintf(stderr, "Could not open x%sx for reading.\n", argv[1]);
          exit(1);
      }
      nseqs = totbases = nbases = saw_multiple = 0;
      while((c = fgetc(fasta)) != EOF) {
          switch(c) {
          case '>':
              if(nbases) {
                if(!output_summary_only && output_length)
                  printf("%d %s", nbases, buf);
                saw_multiple=1;
                nbases = 0;
              }
              nseqs++;
              fgets(buf, 2048, fasta);
              break;
          case ' ': case '\n':
              break;
          default:
              nbases++;
              totbases++;
              break;
          }
      }
      if(saw_multiple) {
        if(!output_summary_only && output_length)
          printf("%d %s\n", nbases, buf);
      }
      if(output_num + output_length + output_mean_length > 1) {
        if(output_summary_only) {
          if(output_num)
            printf("%d\t",nseqs);
          if(output_length)
            printf("%d\t",totbases);
          if(output_mean_length)
            printf("%f\t",(float)totbases/nseqs);
          printf("\n");
        }
        else {
          if(output_num)
            printf("number_of_sequences %d\n", nseqs);
          if(output_length)
            printf("total_length %d\n", totbases);
          if(output_mean_length)
            printf("mean_length %.1f\n", (float) totbases/nseqs);
          printf("\n");
        }
      }
      else {
        if(output_num)
          printf("%d", nseqs);
        else if(output_length)
          printf("%d", totbases);
        else if(output_mean_length)
          printf("%.1f", (float) totbases/nseqs);
        if(argc > 1) 
          printf(" %s\n", argv[i]);
        else
          printf("\n");
      }
      fclose(fasta);
    }
    return 0;
}
