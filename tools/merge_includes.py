#!/usr/bin/python

# Nonius - C++ benchmarking tool
#
# Written in 2014- by the nonius contributors <nonius@rmf.io>
#
# To the extent possible under law, the author(s) have dedicated all copyright and related
# and neighboring rights to this software to the public domain worldwide. This software is
# distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

# Include file merging script

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
