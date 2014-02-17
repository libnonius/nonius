#!/usr/bin/python

import ninja_syntax
import itertools
import os
import fnmatch
import re
import sys
import argparse

# --- util functions

def flags(*iterables):
    return ' '.join(itertools.chain(*iterables))

def include(d):
    return '-I' + d;

def define(d):
    return '-D' + d;

def library(l):
    return '-l' + l;

def dependency_include(d):
    return '-isystem ' + os.path.join('deps', d, 'include');

def get_files(root, pattern):
    pattern = fnmatch.translate(pattern)
    for dir, dirs, files in os.walk(root):
        for f in files:
            if re.match(pattern, f):
                yield os.path.join(dir, f)

def object_file(fn):
    return os.path.join('obj', re.sub(r'\.c\+\+$', '.o', fn))

# --- arguments

parser = argparse.ArgumentParser()
parser.add_argument('--debug', action='store_true', help='compile with debug information')
parser.add_argument('--cxx', default='g++', metavar='executable', help='compiler name to use (default: g++)')
parser.add_argument('--boost-dir', default=None, metavar='path', help='path of boost folder (i.e. the folder with include/ and lib/ subfolders)')
parser.add_argument('--no-lto', action='store_true', help='do not perform link-time optimisation')
args = parser.parse_args()

# --- variables

dependencies = ['catch', 'wheels']
include_flags = flags([include('include')], map(dependency_include, dependencies))
if(args.boost_dir):
    include_flags += ' ' + dependency_include(args.boost_dir)
cxx_flags = flags(['-Wall', '-Wextra', '-pedantic', '-Werror', '-std=c++11', '-pthread', '-g' if args.debug else '-O3'])
define_flags = ''
lib_flags = ''
ld_flags = flags(['-pthread'] + [] if args.no_lto or args.debug else ['-flto'])

# --- preamble

ninja = ninja_syntax.Writer(open('build.ninja', 'w'))

ninja.variable('ninja_required_version', '1.3')
ninja.variable('builddir', 'obj' + os.sep)

# --- rules

ninja.rule('bootstrap',
        command = ' '.join(sys.argv),
        generator = True,
        description = 'BOOTSTRAP')

ninja.rule('cxx',
        command = args.cxx + ' -MMD -MF $out.d -c ' + cxx_flags + ' ' + include_flags + ' ' + define_flags + ' $in -o $out',
        deps = 'gcc',
        depfile = '$out.d',
        description = 'C++ $in')

ninja.rule('link',
        command = args.cxx + ' ' + cxx_flags + ' ' + ld_flags + ' $in -o $out' + ' ' + lib_flags,
        description = 'LINK $in')

ninja.rule('lib',
        command = 'ar rcs $out $in',
        description = 'AR $in')

ninja.rule('header',
        command = './single_header.py $in $out',
        description = 'HEADER $in')

# --- build edges

ninja.build('build.ninja', 'bootstrap',
        implicit = sys.argv[0])

hdr_files = list(get_files('include', '*.h++'))
src_files = list(get_files('src', '*.c++'))
obj_files = [object_file(fn) for fn in src_files]
for fn in src_files:
    ninja.build(object_file(fn), 'cxx',
            inputs = fn)

built_libs = []
if len(obj_files) > 0:
    ninja.build('lib', 'phony',
            inputs = libnonius)
    libnonius = os.path.join('bin', 'libnonius.a')
    ninja.build(libnonius, 'lib',
            inputs = obj_files)
    built_libs = [libnonius]

test_src_files = list(get_files('test', '*.c++'))
test_obj_files = [object_file(fn) for fn in test_src_files]
for fn in test_src_files:
    ninja.build(object_file(fn), 'cxx',
            inputs = fn)

test_runner = os.path.join('bin', 'test')
ninja.build(test_runner, 'link',
        inputs = test_obj_files + built_libs)
ninja.build('test', 'phony',
        inputs = test_runner)

header = os.path.join('dist', 'nonius.h++')
ninja.build(header, 'header',
        inputs = os.path.join('include', 'nonius', 'nonius_single.h++'))

ninja.build('header', 'phony',
        inputs = header)

# --- examples

example_files = list(get_files('examples', '*.c++'))
examples = []
for fn in example_files:
    ninja.build(object_file(fn), 'cxx',
            inputs = fn)
    name = re.sub(r'\.c\+\+$', '', os.path.basename(fn))
    example = os.path.join('bin', 'examples', name)
    ninja.build(example, 'link',
            inputs = [object_file(fn)] + built_libs)
    ninja.build(name, 'phony',
            inputs = example)
    examples += [name]

ninja.build('examples', 'phony',
            inputs = examples)

ninja.default('examples')

