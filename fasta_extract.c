#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void usage (char * progname)
{
    fprintf(stderr, "%s file-name id [ id ... ]\n", progname);
    fprintf(stderr, " -n    id is not id in the header, but ordinal in file\n");
    fprintf(stderr, " -h    display help\n");
    exit(1);
}

int idcmp(char *arg,char * line)
{
  int alen,i;

  alen = strlen(arg);
  if(strlen(line) < alen)
    return 1;

  for(i = 0; i <= alen; i++) {
    if(arg[i] == '*')
      return 0;
    if(isspace(line[i]) && i == alen)
      return 0;
    if(arg[i] != line[i]) 
      return arg[i] - line[i];
  }
  return 0;
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[2048], c;
    int argn,output_num,in_match,seqnum;


    output_num = 0;
    while((c = getopt(argc, argv, "nh")) != -1) {
      switch(c) {
      case 'n':
        output_num = 1;
        break;
      case 'h':
        usage(argv[0]);
        break;
      }
    }

    if(argc - optind < 2) 
        usage(argv[0]);
    argv += optind;
    argc -= optind;

    fasta = fopen(argv[0], "r");

    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[0]);
        exit(1);
    }
    in_match = 0;
    seqnum = 1;
    while((c = fgetc(fasta)) != EOF) {
      switch(c) {
      case '>':
        in_match = 0;
        fgets(buf, 2048, fasta);
          for(argn = 1; argn < argc; argn++) 
            if((output_num && seqnum == strtol(argv[argn],NULL,0)) || 
                (!output_num && !idcmp(argv[argn],buf))) {
              in_match = 1;
              break;
            }
        if(in_match)
          printf(">%s", buf);
        seqnum++;
        break;
      default:
        if(in_match)
          fputc(c,stdout);
        break;
      }
    }
    fclose(fasta);
    return 0;
}
