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

# Script for making C++ strings out of text files

import sys
import os
import re

if len(sys.argv) != 3:
    print('usage: ' + os.path.basename(sys.argv[0]) + ' <input> <output>')

infile = sys.argv[1]
outfile = sys.argv[2]

max_len = 10000

def split_len(s, length):
    return [s[i:i+length] for i in range(0, len(s), length)]

def stringize(s):
    s = s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')
    return '"' + s + '"\n'

with open(infile, 'r') as src, open(outfile, 'w') as dst:
    length = 0
    for line in src:
        for piece in split_len(line, max_len):
            stringized = stringize(piece)
            length += len(stringized)
            if length > max_len:
                dst.write(',\n')
                length = len(stringized)
            dst.write(stringized)

