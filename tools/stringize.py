#!/usr/bin/python

import sys
import os
import re

if len(sys.argv) != 3:
    print('usage: ' + os.path.basename(sys.argv[0]) + ' <input> <output>')

infile = sys.argv[1]
outfile = sys.argv[2]

def stringize(s):
    s = s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')
    return '"' + s + '"\n'

with open(infile, 'r') as src, open(outfile, 'w') as dst:
    for line in src:
        dst.write(stringize(line))

