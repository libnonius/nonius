#!/usr/bin/python

import os
import sys
import re
import datetime
import string

inputPath = sys.argv[1]
rootPath = os.path.dirname(inputPath) + "/"
outputPath = sys.argv[2]
depPath = 'deps/cpptemplate'

includesParser = re.compile( r'\s*#include\s*<nonius/(.*)>' )
guardParser = re.compile( r'\s*#.*NONIUS_.*_HPP')
defineParser = re.compile( r'\s*#define')
ifParser = re.compile( r'\s*#ifndef NONIUS_.*HPP')
endIfParser = re.compile( r'\s*#endif // NONIUS_.*_HPP')
ifImplParser = re.compile( r'\s*#if.*(NONIUS_RUNNER)')
commentParser1 = re.compile( r'^\s*/\*')
commentParser2 = re.compile( r'^\s*\*')
blankParser = re.compile( r'^\s*$')
seenHeaders = set([])

bumpVersion = True
includeImpl = True

out = open( outputPath, 'w' )
ifdefs = 0
implIfDefs = -1

def write( line ):
    if includeImpl or implIfDefs == -1:
        out.write( line )

def parseFile( path, filename ):
    global ifdefs
    global implIfDefs

    f = open( os.path.join(path, filename), 'r' )
    blanks = 0
    for line in f:
        if ifParser.match( line ):
            ifdefs = ifdefs + 1
        elif endIfParser.match( line ):
            ifdefs = ifdefs - 1
        m = includesParser.match( line )
        if m:
            header = m.group(1)
            headerPath, sep, headerFile = header.rpartition( "/" )
            if not headerFile in seenHeaders:
                seenHeaders.add( headerFile )
                write( "// #included from: {0}\n".format( header ) )
                if os.path.exists( path + headerPath + sep + headerFile ):
                    parseFile( path + headerPath + sep, headerFile )
                elif os.path.exists( depPath + sep + headerFile ):
                    parseFile( depPath + sep, headerFile )
                else:
                    parseFile( rootPath + headerPath + sep, headerFile )
        else:
            if ifImplParser.match(line):
                implIfDefs = ifdefs
            if (not guardParser.match( line ) or defineParser.match( line ) ) and not commentParser1.match( line )and not commentParser2.match( line ):
                if blankParser.match( line ):
                    blanks = blanks + 1
                else:
                    blanks = 0
                if blanks < 2:
                    write( line.rstrip() + "\n" )

def generateSingleInclude():
    out.write("""// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// This file was automatically generated on {0}
// Do not edit it directly

""".format(datetime.datetime.utcnow().isoformat()+'Z'))

    out.write( "#ifndef NONIUS_SINGLE_INCLUDE_HPP\n" )
    out.write( "#define NONIUS_SINGLE_INCLUDE_HPP\n\n" )

    parseFile( rootPath, 'nonius_single.h++' )

    out.write( "#endif // NONIUS_SINGLE_INCLUDE_HPP\n" )

generateSingleInclude()

