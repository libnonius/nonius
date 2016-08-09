#!/usr/bin/python

import sys
import os
import re
from os import path

if len(sys.argv) != 3:
    print('usage: ' + os.path.basename(sys.argv[0]) + ' <input> <output>')

infile = sys.argv[1]
outfile = sys.argv[2]

basedir = path.dirname(infile)

def expand_match(match):
    fname = path.join(basedir, match.group(1))
    with open(fname, 'r') as included:
        return included.read()

with open(infile, 'r') as src, open(outfile, 'w') as dst:
    contents = src.read()
    merged   = re.sub(r'\{%\s*include\s*"(.*)"\s*%\}', expand_match, contents)
    dst.write(merged)
