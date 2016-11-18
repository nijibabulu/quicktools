#include <ctype.h>
/* -- taken from jksrc -- */

#define TRUE 1
#define FALSE 0
#define boolean int
static boolean inittedNtVal = FALSE;

#define ArraySize(a) (sizeof(a)/sizeof((a)[0]))

/* Numerical values for bases. */
#define T_BASE_VAL 0
#define U_BASE_VAL 0
#define C_BASE_VAL 1
#define A_BASE_VAL 2
#define G_BASE_VAL 3
#define N_BASE_VAL 4   /* Used in 1/2 byte representation. */

typedef char DNA;
typedef char AA;

#define MASKED_BASE_BIT 8

struct codonTable
/* The dread codon table. */
    {
    DNA *codon;		/* Lower case. */
    AA protCode;	/* Upper case. The "Standard" code */
    AA mitoCode;	/* Upper case. Vertebrate Mitochondrial translations */
    };

struct codonTable codonTable[] = 
/* The master codon/protein table. */
{
    {"ttt", 'F', 'F',},
    {"ttc", 'F', 'F',},
    {"tta", 'L', 'L',},
    {"ttg", 'L', 'L',},

    {"tct", 'S', 'S',},
    {"tcc", 'S', 'S',},
    {"tca", 'S', 'S',},
    {"tcg", 'S', 'S',},

    {"tat", 'Y', 'Y',},
    {"tac", 'Y', 'Y',},
    {"taa", 0, 0,},
    {"tag", 0, 0,},

    {"tgt", 'C', 'C',},
    {"tgc", 'C', 'C',},
    {"tga", 0, 'W',},
    {"tgg", 'W', 'W',},


    {"ctt", 'L', 'L',},
    {"ctc", 'L', 'L',},
    {"cta", 'L', 'L',},
    {"ctg", 'L', 'L',},

    {"cct", 'P', 'P',},
    {"ccc", 'P', 'P',},
    {"cca", 'P', 'P',},
    {"ccg", 'P', 'P',},

    {"cat", 'H', 'H',},
    {"cac", 'H', 'H',},
    {"caa", 'Q', 'Q',},
    {"cag", 'Q', 'Q',},

    {"cgt", 'R', 'R',},
    {"cgc", 'R', 'R',},
    {"cga", 'R', 'R',},
    {"cgg", 'R', 'R',},


    {"att", 'I', 'I',},
    {"atc", 'I', 'I',},
    {"ata", 'I', 'M',},
    {"atg", 'M', 'M',},

    {"act", 'T', 'T',},
    {"acc", 'T', 'T',},
    {"aca", 'T', 'T',},
    {"acg", 'T', 'T',},

    {"aat", 'N', 'N',},
    {"aac", 'N', 'N',},
    {"aaa", 'K', 'K',},
    {"aag", 'K', 'K',},

    {"agt", 'S', 'S',},
    {"agc", 'S', 'S',},
    {"aga", 'R', 0,},
    {"agg", 'R', 0,},


    {"gtt", 'V', 'V',},
    {"gtc", 'V', 'V',},
    {"gta", 'V', 'V',},
    {"gtg", 'V', 'V',},

    {"gct", 'A', 'A',},
    {"gcc", 'A', 'A',},
    {"gca", 'A', 'A',},
    {"gcg", 'A', 'A',},

    {"gat", 'D', 'D',},
    {"gac", 'D', 'D',},
    {"gaa", 'E', 'E',},
    {"gag", 'E', 'E',},

    {"ggt", 'G', 'G',},
    {"ggc", 'G', 'G',},
    {"gga", 'G', 'G',},
    {"ggg", 'G', 'G',},
};

/* A table that gives values 0 for t
			     1 for c
			     2 for a
			     3 for g
 * (which is order aa's are in biochemistry codon tables)
 * and gives -1 for all others. */
int ntVal[256];
int ntValLower[256];	/* NT values only for lower case. */
int ntValUpper[256];	/* NT values only for upper case. */
int ntVal5[256];
int ntValNoN[256]; /* Like ntVal, but with T_BASE_VAL in place of -1 for nonexistent ones. */
DNA valToNt[(N_BASE_VAL|MASKED_BASE_BIT)+1];

/* convert tables for bit-4 indicating masked */
int ntValMasked[256];
DNA valToNtMasked[256];


static void initNtVal()
{
if (!inittedNtVal)
    {
    int i;
    for (i=0; i<ArraySize(ntVal); i++)
        {
	ntValUpper[i] = ntValLower[i] = ntVal[i] = -1;
        ntValNoN[i] = T_BASE_VAL;
	if (isspace(i) || isdigit(i))
	    ntVal5[i] = ntValMasked[i] = -1;
	else
            {
	    ntVal5[i] = N_BASE_VAL;
	    ntValMasked[i] = (islower(i) ? (N_BASE_VAL|MASKED_BASE_BIT) : N_BASE_VAL);
            }
        }
    ntVal5['t'] = ntVal5['T'] = ntValNoN['t'] = ntValNoN['T'] = ntVal['t'] = ntVal['T'] = 
    	ntValLower['t'] = ntValUpper['T'] = T_BASE_VAL;
    ntVal5['u'] = ntVal5['U'] = ntValNoN['u'] = ntValNoN['U'] = ntVal['u'] = ntVal['U'] = 
    	ntValLower['u'] = ntValUpper['U'] = U_BASE_VAL;
    ntVal5['c'] = ntVal5['C'] = ntValNoN['c'] = ntValNoN['C'] = ntVal['c'] = ntVal['C'] = 
    	ntValLower['c'] = ntValUpper['C'] = C_BASE_VAL;
    ntVal5['a'] = ntVal5['A'] = ntValNoN['a'] = ntValNoN['A'] = ntVal['a'] = ntVal['A'] = 
    	ntValLower['a'] = ntValUpper['A'] = A_BASE_VAL;
    ntVal5['g'] = ntVal5['G'] = ntValNoN['g'] = ntValNoN['G'] = ntVal['g'] = ntVal['G'] = 
    	ntValLower['g'] = ntValUpper['G'] = G_BASE_VAL;

    valToNt[T_BASE_VAL] = valToNt[T_BASE_VAL|MASKED_BASE_BIT] = 't';
    valToNt[C_BASE_VAL] = valToNt[C_BASE_VAL|MASKED_BASE_BIT] = 'c';
    valToNt[A_BASE_VAL] = valToNt[A_BASE_VAL|MASKED_BASE_BIT] = 'a';
    valToNt[G_BASE_VAL] = valToNt[G_BASE_VAL|MASKED_BASE_BIT] = 'g';
    valToNt[N_BASE_VAL] = valToNt[N_BASE_VAL|MASKED_BASE_BIT] = 'n';

    /* masked values */
    ntValMasked['T'] = T_BASE_VAL;
    ntValMasked['U'] = U_BASE_VAL;
    ntValMasked['C'] = C_BASE_VAL;
    ntValMasked['A'] = A_BASE_VAL;
    ntValMasked['G'] = G_BASE_VAL;

    ntValMasked['t'] = T_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['u'] = U_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['c'] = C_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['a'] = A_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['g'] = G_BASE_VAL|MASKED_BASE_BIT;

    valToNtMasked[T_BASE_VAL] = 'T';
    valToNtMasked[C_BASE_VAL] = 'C';
    valToNtMasked[A_BASE_VAL] = 'A';
    valToNtMasked[G_BASE_VAL] = 'G';
    valToNtMasked[N_BASE_VAL] = 'N';

    valToNtMasked[T_BASE_VAL|MASKED_BASE_BIT] = 't';
    valToNtMasked[C_BASE_VAL|MASKED_BASE_BIT] = 'c';
    valToNtMasked[A_BASE_VAL|MASKED_BASE_BIT] = 'a';
    valToNtMasked[G_BASE_VAL|MASKED_BASE_BIT] = 'g';
    valToNtMasked[N_BASE_VAL|MASKED_BASE_BIT] = 'n';

    inittedNtVal = TRUE;
    }
}

AA lookupCodon(DNA *dna)
{
int ix;
int i;
char c;

if (!inittedNtVal)
    initNtVal();
ix = 0;
for (i=0; i<3; ++i)
    {
    int bv = ntVal[(int)dna[i]];
    if (bv<0)
	return 'X';
    ix = (ix<<2) + bv;
    }
c = codonTable[ix].protCode;
return c;
}

/* -- end taken from jksrc -- */
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

void usage (char * progname)
{
    fprintf(stderr, "%s file-name\n", progname);
    fprintf(stderr, "Converts all lowercase DNA characters into 'N's\n");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    FILE * fasta;
    char buf[256], codon_buf[3], codon_pos = 0, c, aa;

    if(argc != 2) 
        usage(argv[0]);

    fasta = fopen(argv[1], "r");
    if(fasta == NULL) {
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        exit(1);
    }
    while((c = fgetc(fasta)) != EOF) {
        switch(c) {
        case '>':
            fgets(buf, 256, fasta);
            printf(">%s", buf);
            codon_pos = 0;
            break;
        case '\n':
            putchar(c);
            break;
        default:
            codon_buf[codon_pos] = tolower(c);
            codon_pos++;
            if(codon_pos == 3) {
              aa = lookupCodon(codon_buf);
              if(aa != 0)
                /*aa = '*';*/
                putchar(aa);
              codon_pos = 0;
            }
            break;
        }
    }
    fclose(fasta);
    return 0;
}

