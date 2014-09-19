#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>

void usage (char * progname)
{
    fprintf(stderr, "%s [OPTIONS] file-name\n", progname);
    fprintf(stderr, "Split a fasta file into smaller fasta files.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -n NUM    split into NUM files\n");
    fprintf(stderr, " -w NUM    wrap lines at length given, 0 to never wrap\n");
    fprintf(stderr, " -s NUM    split into files with NUM sequences each\n");
    fprintf(stderr, " -o DIR    use DIR for the output directory, not CWD\n");
    fprintf(stderr, " -1        split each sequence to a separate file (default).\n");
    fprintf(stderr, " -h\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    extern char *optarg;
    extern int optind;
    FILE * fasta, *out;
    char base[2048],name[2048], filename[2048], buf[2048], c,*s,*bn,*outdir;
    int calc,linepos,i,n,each,nseqs,consumed_seqs,fileno,wrap;
    unsigned long len;

    each=1;
    n=0;
    calc=0;
    wrap=60;
    outdir=NULL;
    while((c = getopt(argc, argv, "o:w:n:s:1h")) != -1) {
      switch(c) {
      case 'o': outdir=optarg; break;
      case 'w':
        wrap = strtol(optarg,NULL,0);
        break;
      case 'n':
      case 's':
        if(n) {
          fprintf(stderr, "-n and -s are mutually exclusive\n");
          exit(1);
        }
        n = strtol(optarg,NULL,0);
        if(c == 'n') 
          calc = 1;
        each = 0;
        break;
      case '1':
        if(n) {
          fprintf(stderr, "-1 and -n and -s are mutually exclusive\n");
          exit(1);
        }
        each = 1; break;
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

    if(outdir == NULL) 
      base[0] = '\0';
    else {
      strcpy(base,outdir);
      strcat(base,"/");
    }


    bn = basename(argv[0]);
    for(s=bn+strlen(bn); *s != '.' && s != bn; s--) {
      continue;
    }

    if(s == bn)
      strcat(base, basename(argv[0]));
    else {
      len=(unsigned long)s-(unsigned long)bn;
      strncat(base,basename(argv[0]),len);
      base[len]='\0';
    }


    if(calc) {
      for(nseqs=0; (c = fgetc(fasta)) != EOF;) 
        if(c == '>')
          nseqs++;
      n = nseqs/n;
      fseek(fasta,0L,SEEK_SET);
    }
    out = NULL;
    fileno = 0;
    consumed_seqs=0; /* then the first ++ makes it 1 */
    while((c = fgetc(fasta)) != EOF) {
        switch(c) {
        case '>':
            fgets(name, 2048, fasta);
            if(each) {
              strcpy(filename, name);
              for(i = 0; i < 2048; i++) {
                if(filename[i] == '\n' || filename[i] == '\r' || isspace(filename[i]) || filename[i] == '/') {
                  filename[i] = '\0';
                  break;
                }
              }
              if(i >= 2000) {
                fprintf(stderr, "problem with file...no eol on header.\n"); 
                exit(1);
              }
              strcat(filename,".fa");
              if(outdir != NULL) {
                strcpy(buf,filename);
                sprintf(filename, "%s/%s", outdir,buf);
              }
            }
            else {
              if(consumed_seqs==n) 
                consumed_seqs = 1;
              else
                consumed_seqs++;
              /* the first time and each time it is set */
              if(consumed_seqs == 1) { 
                fileno++; 
                sprintf(filename, "%s.%d.fa",base,fileno);
              }
            }
            if(out != NULL && linepos)  fputc('\n', out);  
            if(each || consumed_seqs == 1) {
              if(out != NULL) fclose(out);
              out = fopen(filename, "w");
            }
            fprintf(out, ">%s",name);
            linepos=0;
            break;
        case ' ': case '\n': case '\r':
            break;
        default:
            if(out == NULL) {
              fprintf(stderr, "No header found!\n");
              exit(1);
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
