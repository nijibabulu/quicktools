#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>
#include<unistd.h>
#include<zlib.h>

#define BUFSZ 32768
void usage (char * progname)
{
    fprintf(stderr, "%s [OPTIONS] NUM FASTQ\n", progname);
    fprintf(stderr, "Split a fastq file into smaller fastq files.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -n     split into NUM files\n");
    fprintf(stderr, " -s     split into files with NUM sequences each [default]\n");
    fprintf(stderr, " -z     input file is gzip compressed (this is \n");
    fprintf(stderr, "        automatically set if the file ends in .gz\n");
    fprintf(stderr, " -o DIR use DIR for the output directory, not CWD\n");
    fprintf(stderr, " -h\n");
    exit(1);
}

int zmode = 0;

void *
xfopen(const char *path, const char *mode)
{
  if(zmode) { return (void *) gzopen(path,mode); }
  else      { return (void *) fopen(path,mode); }
}

int
xfseek(void *f, long offset, int whence)
{
  if(zmode) { return gzseek((gzFile) f, (z_off_t) offset, whence); }
  else      { return fseek((FILE *) f, offset,whence); }
}

int
xfclose(void *f, int write_mode)
{
  if(zmode) { 
    if(write_mode) { return gzclose_w((gzFile) f); }
    else           { return gzclose_r((gzFile) f); }
  }
  else {
    return fclose((FILE *) f);
  }
}

int
xfputs(const char *s, void *f)
{
  if(zmode) { return gzputs((gzFile) f, s); }
  else      { return fputs(s, (FILE *) f); }
}

char *
xfgets(char *buf, int len, void *f)
{
  if(zmode) { return gzgets((gzFile) f, buf, len); }
  else      { return fgets(buf, len, (FILE *) f); }
}

int
check_binary(const char * filename) 
{
  FILE *f = fopen(filename,"rb");
  int i,c;
  if(f == NULL) {
    fprintf(stderr, "Could not open %s for reading\n", filename);
    exit(1);
  }
  for(i = 0; (c = fgetc(f)) && i < BUFSZ; i++) {
    if(c > 127 || c == 0)  {
      fclose(f);
      return 1;
    }
  }
  fclose(f);
  return 0;
}

void *
new_outfile(void * cur_outfile, const char * base, int fileno)
{
  char filename[BUFSZ];
  if(cur_outfile != NULL) 
    xfclose(cur_outfile, 1);
  sprintf(filename, "%s.%d.fastq",base,fileno);
  if(zmode) 
    strcat(filename, ".gz");
  return xfopen(filename,"w");
}

int
gzip_suffix(const char * name) 
{
  return (strcmp(".gz",name + (strlen(name)-3)) == 0);
}

int main(int argc, char *argv[]) 
{
    extern char *optarg;
    extern int optind;
    FILE * fastq, *out;
    char base[BUFSZ], buf[BUFSZ], c,*s,*bn,*outdir,*fastq_name;
    int calc,n,nseqs,consumed_seqs,fileno,num_files,seqpos;
    unsigned long len;

    n=0;
    calc=0;
    outdir=NULL;
    while((c = getopt(argc, argv, "o:nzsh")) != -1) {
      switch(c) {
      case 'o': outdir=optarg; break;
      case 'z': zmode=1; break;
      case 'n':
      case 's':
        if(n) {
          fprintf(stderr, "-n and -s are mutually exclusive\n");
          exit(1);
        }
        if(c == 'n') 
          calc = 1;
        break;
      case 'h':
      default:
        usage(argv[0]);
        break;
      }
    }
    
    if(argc-optind != 2) 
        usage(argv[0]);

    argv += optind;
    argc -= optind;

    num_files = n = strtol(argv[0],NULL,0);
    fastq_name = argv[1];
    if(gzip_suffix(fastq_name))
      zmode = 1;
    if(check_binary(fastq_name) == 1 && zmode == 0) {
      fprintf(stderr, "The file %s appears to be a binary ", fastq_name);
      fprintf(stderr, "file but we are not in gzip mode. Exiting.\n");
      exit(1);
    }
    fastq = xfopen(fastq_name, "r");
    if(fastq == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[0]);
        exit(1);
    }

    if(outdir == NULL) 
      base[0] = '\0';
    else {
      strcpy(base,outdir);
      strcat(base,"/");
    }

    bn = basename(fastq_name);
    for(s=bn+strlen(bn); *s != '.' && s != bn; s--);
    if(gzip_suffix(fastq_name)) 
      for(s--; *s != '.' && s != bn; s--);
    if(s == bn)
      strcat(base, basename(fastq_name));
    else {
      len=strlen(base) + (unsigned long)s-(unsigned long)bn;
      strncat(base,basename(fastq_name),len);
      base[len]='\0';
    }


    if(calc) {
      for(nseqs=0;  xfgets(buf,BUFSZ,fastq) != NULL; nseqs++) ;
      if(nseqs % 4 != 0) {
        fprintf(stderr, "Format error: fastq file lines are not divisible by 4\n");
        exit(1);
      }
      nseqs /= 4;
      n = nseqs/num_files;
      xfseek(fastq,0L,SEEK_SET);
    }
    out = NULL;
    fileno = 0;
    consumed_seqs=0; /* then the first ++ makes it 1 */
    for(seqpos = 0; xfgets(buf,BUFSZ,fastq) != NULL; seqpos = (seqpos + 1) % 4) {
        if(out == NULL) {
          out = new_outfile(NULL, base,++fileno);
        } else if(seqpos == 0) {
          consumed_seqs ++;
          /* the last file may have more sequences than n so don't bump
           * the file number in this case */
          if(consumed_seqs % n == 0 && !(calc && fileno == num_files)) 
            out = new_outfile(out,base,++fileno);
        }
        xfputs(buf, out);
    }
    xfclose(out,1);
    xfclose(fastq,0);
    return 0;
}
