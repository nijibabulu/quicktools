#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>

void usage (char * progname)
{
    fprintf(stderr, "%s [OPTIONS] file-name [ file-name ... ]\n", progname);
    fprintf(stderr, "Split a fasta file into smaller fasta files.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -d STR    split by the given delimiter characters [Nn]\n");
    fprintf(stderr, " -n NUM    split only if at least NUM characters in a row\n");
    fprintf(stderr, "           are delimiter characters.\n");
    fprintf(stderr, " -h\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    extern char *optarg;
    extern int optind;
    FILE * fasta, *out;
    char base[2048],name[2048], filename[2048], c,*delim;
    int i,linepos,n,wrap;
    unsigned long len, last, pos;

    delim = "Nn";

    n=60;
    wrap=60;
    while((c = getopt(argc, argv, "d:h")) != -1) {
      switch(c) {
      case 'd':
        delim = optarg;
        break;
      case 'n':
        n = strtol(optarg,NULL,0);
        break;
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

    while((c = fgetc(fasta)) != EOF) {
        switch(c) {
        case '>':
            fgets(name, 2048, fasta);
            for(i = 0; i < 2048; i++) {
              if(name[i] == '\n' || name[i] == '\r' || isspace(name[i]) || name[i] == '/') {
                name[i] = '\0';
                break;
              }
            }
            if(i >= 2000) {
              fprintf(stderr, "problem with file...no eol on header.\n"); 
              exit(1);
            }
            strcpy(filename,name);
            strcat(filename,".0.fa");
            if(out != NULL)
              fclose(out);
            out = fopen(filename,"w");
            fprintf(out, ">%s.0\n",name);
            linepos=0;
            pos = 0;
            break;
        case ' ': case '\n': case '\r':
            break;
        default:
            if(out == NULL) {
              fprintf(stderr, "No header found!\n");
              exit(1);
            }
            pos ++;
            if(strchr(delim,c) == NULL) {
              if(pos-last > n) {
                printf("%lu %lu\n", pos, pos-last);
                sprintf(filename,"%s.%lu.fa", name,pos);
                fclose(out);
                out = fopen(filename,"w");
                fprintf(out,">%s.%lu\n",name,pos);
                linepos = 0;
              }
              last = pos;
            }
            fputc(c,out);
            linepos++;
            if (wrap > 0 && linepos == wrap) {
              linepos = 0;
              fputc('\n',out);
            }
            break;
        }
    }
    fclose(out);
    fclose(fasta);
    return 0;
}
