PROGS=fasta_translate fasta_split fastq_split fasta_length fasta_substr fasta_sample lcmask ucfasta simple_tm fasta_names fasta_stat add subtract mean max min stddev stderror randseq seqfmt make_fasta fasta_extract contig_split fasta_rc mw fasta_uniqueid fasta_range
PYPROGS=fasta_histogram sam_to_fastx

CC=gcc
CFLAGS?=-g -Wno-unused-result 
LDFLAGS=-lm -lz
DESTDIR=/usr

%: %.c
	$(CC) $(CFLAGS) -o $@ $@.c $(LDFLAGS) 

%: %.py
	install -m 0755 $@.py $@

all: $(PROGS) $(PYPROGS)

ifneq ($(shell uname), Darwin)
RANDSEQ_LIBS= -lrt
endif

randseq: randseq.c
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $@.c $(RANDSEQ_LIBS)

install: all
	install -d $(DESTDIR)/bin
	@for f in $(PROGS) $(PYPROGS) ; do \
		echo "installing $$f"; \
		install -m 0755 $$f $(DESTDIR)/bin; \
	done
clean:
	rm -f $(PROGS) $(PYPROGS)
.PHONY: all
