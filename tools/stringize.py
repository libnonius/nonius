#!/usr/bin/python

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

