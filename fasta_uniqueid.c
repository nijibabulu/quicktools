#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>

void usage (char * progname)
{
    fprintf(stderr, "%s [OPTIONS] file-name\n", progname);
    fprintf(stderr, "Change IDs of a fasta file to short, unique IDs.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -p PRE    use PRE for prefix. [default: seqid]\n");
    fprintf(stderr, " -m FILE   output map of old names to new names to FILE.\n");
    fprintf(stderr, " -h\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    extern char *optarg;
    extern int optind;
    FILE * fasta, *mapout;
    char filename[2048], old_name[2048], new_name[2048],  *id_prefix, c;
    int seqnum, i;
    unsigned long len;

    id_prefix="seqid";
    mapout=NULL;
    while((c = getopt(argc, argv, "m:p:h")) != -1) {
      switch(c) {
      case 'm': 
        mapout = fopen(optarg,"w");
        if(mapout == NULL) {
          fprintf(stderr, "Could not open %s for writing.\n", optarg);
          exit(1);
        }
        break;
      case 'p': id_prefix = optarg; break;
      case 'h':
      default:
        usage(argv[0]);
        break;
      }
    }
    
    if(argc-optind != 1) 
        usage(argv[0]);

    argv += optind;
    argc -= optind;

    fasta = fopen(argv[0], "r");
    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[0]);
        exit(1);
    }

    seqnum = 1;
    while((c = fgetc(fasta)) != EOF) {
        switch(c) {
        case '>':
            fgets(old_name, 2048, fasta);
            sprintf(new_name, "%s_%d", id_prefix, seqnum++);
            printf(">%s\n", new_name);
            if(mapout != NULL) {
              for(i = strlen(old_name)-1; isspace(old_name[i]); i--) 
                old_name[i] = '\0';
              fprintf(mapout,"%s %s\n", old_name,new_name);
            }
            break;
        default:
            fputc(c,stdout);
            break;
        }
        
    }
    fclose(fasta);
    return 0;
}
