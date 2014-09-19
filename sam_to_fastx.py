#! /usr/bin/env python

import sys
import pysam
import argparse

parser = argparse.ArgumentParser(description='''convert SAM/BAM file to fastq
                                 format or fasta format. Cigars are ignored and
                                 the query sequences are outputted''')
parser.add_argument('file',nargs='+')
parser.add_argument('--format',choices=['fastq','fasta'],default='fastq')
args = parser.parse_args()

for file in args.file:
    if file == '-':
        sf = pysam.Samfile('/dev/stdin')
    else:
        sf = pysam.Samfile(file)

    for aln in sf:
        if args.format == 'fastq':
            print '@%s\n%s\n+\n%s' % (aln.qname,aln.seq,aln.qual)
        elif args.format == 'fasta':
            print '>%s\n%s' % (aln.qname,aln.seq)

