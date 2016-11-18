/* not such a quickie--but for now just here 
 */

#include<ctype.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void
usage() 
{
  fprintf(stderr, "Usage: seqfmt [options] (file|-)\n");
  fprintf(stderr, "Format a fasta or raw sequence file.\n");
  fprintf(stderr, "\n");
  
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h       display this help message and exit\n");
  fprintf(stderr, "  -w cols  wrap at a specified number of columns.\n");
  fprintf(stderr, "           default is 60. (0 to never wrap)\n");
  fprintf(stderr, "  -i       reformat file inplace\n");
  fprintf(stderr, "  -n arg   use the header given instead of file's header\n");
  fprintf(stderr, "  -c       count the id of the header, if this is a \n");
  fprintf(stderr, "           multi-fasta file. The header should have a \n");
  fprintf(stderr, "           %%d somewhere in it\n");
  fprintf(stderr, "  -s spac  add spac extra newlines after each line\n");
  fprintf(stderr, "  -l       number the ends of each line showing position\n");
  fprintf(stderr, "  -d n     specify n for the space to give numbers [5]\n");
  fprintf(stderr, "  -p       crop FASTA header description (after space)\n");
    
  exit(1);
}

int
main(int argc, char *argv[])
{
  int col,width,inplace,count,seqcount,spacing,numbering,i,number_places,seqpos;
  FILE *out,*fasta;
  char buf[16384],c,tmpfilename[20],*header,found,crop;
  
  extern int optind;
  extern char *optarg;

  count = 0;
  header = NULL;
  out = stdout;
  width = 60;
  inplace = 0;
  numbering = 0;
  number_places = 5;
  spacing = 1;
  crop = 0;
  
  while((c = getopt(argc,argv, "cihw:n:s:ld:p")) != -1) {
    switch(c) {
    case 'n': header = optarg; break;
    case 'c': count = 1; break;
    case 'w': width = strtol(optarg,NULL,0); break;
    case 'i': inplace = 1; break;
    case 's': spacing = strtol(optarg, NULL, 0); break;
    case 'l': numbering = 1; break;
    case 'd': number_places = strtol(optarg, NULL, 0); break;
    case 'p': crop = 1; break;
    case 'h': default: usage(); break;
    }
  }

  argc -= optind;
  argv += optind;
  
  if(argc != 1) usage();

  if(strcmp(argv[0],"-") == 0) 
    fasta = stdin;
  else {
    fasta = fopen(argv[0],"r");
    if(fasta == NULL) {
      perror("Could not open fasta file");
      exit(1);
    }

    if(inplace) {/* inplace doesn't make sense with stdin; just put to stdout */
      strcpy(tmpfilename,"fastafmtXXXXXX");
      out = fdopen(mkstemp(tmpfilename),"w");
    }
  }

  seqcount = 0;
  
  while((c = fgetc(fasta)) != EOF) {
    switch(c) {
    case '>':
      seqcount++;
      fgets(buf, 16383, fasta);
      if(header != NULL) strcpy(buf,header);
      if(col > 0) {
        if(numbering) fprintf(out, " %d", seqpos-1);
        if(seqcount) for(i = 0; i < spacing; i++) fputc('\n', out);
      }
      if(count) sprintf(buf,buf,seqcount);
      for(found = 0, i = 0; buf[i] != '\0'; i++) {
        if(buf[i] == '\n')  found = 1;
        if(crop && isspace(buf[i])) break;
      }
      if(!found) { buf[i] = '\n'; buf[i+1] = '\0'; }
      fprintf(out,">%s", buf); /* should include newline -- this is easier :) */
      for(i = 0; i < spacing-1; i++) fputc('\n', out);
      seqpos = 1;
      if(numbering) fprintf(out, "%*d ", number_places, seqpos);
      col = 0;
      break;
    case ' ': case '\n': case '\t': case '\r':
      break;
    default:
      if(col > 0 && width > 0 && col % width == 0) {
        if(numbering) fprintf(out, " %d", seqpos-1);
        for(i = 0; i < spacing; i++) fputc('\n', out);
        if(numbering) fprintf(out, "%*d ", number_places, seqpos);
      }
      col++;
      seqpos++;
      fputc(c,out);
      break;
    }
  }
  if(col > 0) {
    if(numbering) fprintf(out, " %d", seqpos-1);
    fputc('\n', out);
  }

  if(inplace && fasta != stdin) {
    fclose(out);
    out = fopen(tmpfilename,"r");
    fclose(fasta);
    fasta = fopen(argv[0],"w");
    while((c = fgetc(out)) != EOF)
      fputc(c,fasta);
    fclose(fasta);
    fclose(out);
    unlink(tmpfilename);
  }
    
  return 0;
}
