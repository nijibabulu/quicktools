#! /usr/bin/env python

import sys
import os
from optparse import OptionParser,Option
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
from Bio import SeqIO


option_list = [
    Option('','--output',help='''Specify an output file.  The default is the 
basename of the fasta-file if there is only one 
input, or seqs if multiple are given'''),
    Option('','--format',default='pdf',help='format file name suffix [pdf]'),
]

parser = OptionParser(
    'usage: %prog fasta-file [fasta-file ... ]',
    option_list=option_list,
    description='Draw a histogram length distribution of sequences'
)
opts,args = parser.parse_args()
if len(args) < 1:
    parser.error('please provide at least one fasta file')

dist = []
for file in args:
    dist += [len(sr.seq) for sr in SeqIO.parse(open(file), 'fasta')]


# print_figure() got multiple values for keyword argument 'facecolor'
#plt.hist(dist,bins=max(dist),facecolor='green',edgecolor='none')
x = []
y = []
for l in range(max(dist)):
    x += [l]
    y += [len(filter(lambda x: x == l,dist))]
plt.bar(x,y,facecolor='green',edgecolor='green')
#print dist
#print max(dist)

filename = opts.output 
if filename is None:
    if len(args) == 1:
        filename = os.path.join(os.path.basename(args[0]).split('.')[0])
    else:
        filename = 'seqs'

plt.savefig('%s.%s'% (filename,opts.format))
