#!/usr/bin/python

import itertools
import os
import fnmatch
import re
import sys
import argparse

import ninja_syntax
import gcc
import llvm
import msvc

# --- util functions

def flags(*iterables):
    return ' '.join(itertools.chain(*iterables))

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
parser.add_argument('--cxx', default=None, metavar='executable', help='compiler name to use (default depends on toolchain)')
parser.add_argument('--gcc', action='store_true', help='use a GCC toolchain (default)')
parser.add_argument('--llvm', action='store_true', help='use an LLVM toolchain')
parser.add_argument('--msvc', action='store_true', help='use an MSVC toolchain')
parser.add_argument('--boost-dir', default=None, metavar='path', help='path of boost folder (i.e. the folder with include/ and lib/ subfolders)')
parser.add_argument('--no-lto', action='store_true', help='do not perform link-time optimisation')
args = parser.parse_args()

tools = msvc.Toolchain() if args.msvc else llvm.Toolchain() if args.llvm else gcc.Toolchain()
compiler = args.cxx if args.cxx else tools.compiler()
linker = args.cxx if args.cxx else tools.linker()

# --- variables

dependencies = ['catch', 'wheels']
include_flags = flags([tools.include('include')],
                      (tools.dependency_include(os.path.join('deps', d, 'include')) for d in dependencies),
                      [tools.dependency_include('deps/cpptemplate')])
if(args.boost_dir):
    include_flags += ' ' + tools.dependency_include(os.path.join(args.boost_dir, 'include'))
cxx_flags = flags(tools.cxx_flags(),
                  tools.debug_flags() if args.debug else tools.optimisation_flags(),
                  [] if args.no_lto or args.debug else tools.linker_lto_flags())
warning_flags = flags(tools.max_warnings())
define_flags = ''
lib_flags = ''
ld_flags = flags(tools.link_flags(),
                 [] if args.no_lto or args.debug else tools.linker_lto_flags())

stringize_tool = 'tools/stringize.py' 
single_header_tool = 'tools/single_header.py'

# --- preamble

ninja = ninja_syntax.Writer(open('build.ninja', 'w'))

ninja.variable('ninja_required_version', '1.3')
ninja.variable('builddir', 'obj' + os.sep)
ninja.variable('msvc_deps_prefix', 'Note: including file:')
 
# --- rules

ninja.rule('bootstrap',
        command = ' '.join(['python'] + sys.argv),
        generator = True,
        description = 'BOOTSTRAP')

ninja.rule('cxx',
        command = ' '.join([compiler, flags(tools.dependencies_output('$out.d')), cxx_flags, warning_flags, include_flags, define_flags, '$extraflags', '$in', flags(tools.compiler_output('$out'))]),
        deps = tools.ninja_deps_style(),
        depfile = '$out.d',
        description = 'C++ $in')

ninja.rule('link',
        command = ' '.join([linker, ld_flags, '$in', flags(tools.linker_output('$out')), lib_flags]),
        description = 'LINK $out')

ninja.rule('stringize',
        command = ' '.join(['python', stringize_tool, '$in', '$out']),
        description = 'STRINGIZE $in')

ninja.rule('header',
        command = ' '.join(['python', single_header_tool, '$in', '$out']),
        description = 'HEADER $out')

ninja.rule('site',
        command = ' '.join(['jekyll', 'build', '--quiet', '--source', '$in', '--destination', '$out']),
        description = 'JEKYLL $out')

# --- build edges

ninja.build('build.ninja', 'bootstrap',
        implicit = sys.argv[0])

hdr_files = list(get_files('include', '*.h++'))
src_files = list(get_files('src', '*.c++'))
obj_files = [object_file(fn) for fn in src_files]
for fn in src_files:
    ninja.build(object_file(fn), 'cxx',
            inputs = fn)

test_src_files = list(get_files('test', '*.c++'))
test_obj_files = [object_file(fn) for fn in test_src_files]
for fn in test_src_files:
    ninja.build(object_file(fn), 'cxx',
            inputs = fn)

test_runner = os.path.join('bin', 'test') + tools.executable_extension()
ninja.build(test_runner, 'link',
        inputs = test_obj_files)
ninja.build('test', 'phony',
        inputs = test_runner)

html_report_template = 'include/nonius/detail/html_report_template.g.h++'
ninja.build(html_report_template, 'stringize',
        inputs = 'tpl/html_report.tpl',
        implicit = stringize_tool)

ninja.build('templates', 'phony',
        inputs = html_report_template)

header = os.path.join('dist', 'nonius.h++')
ninja.build(header, 'header',
        inputs = os.path.join('include', 'nonius', 'nonius_single.h++'),
        implicit = single_header_tool,
        order_only = 'templates')

ninja.build('header', 'phony',
        inputs = header)

# --- examples

example_files = list(get_files('examples', '*.c++'))
examples = []
for fn in example_files:
    ninja.build(object_file(fn), 'cxx',
            inputs = fn,
            implicit = header,
            variables = { 'extraflags': tools.include('dist') },
            order_only = 'templates')
    name = re.sub(r'\.c\+\+$', '', os.path.basename(fn))
    example = os.path.join('bin', 'examples', name) + tools.executable_extension()
    ninja.build(example, 'link',
            inputs = [object_file(fn)])
    ninja.build(name, 'phony',
            inputs = example)
    examples += [name]

ninja.build('examples', 'phony',
            inputs = examples)

# --- documentation

site = os.path.join('dist', 'doc')
site_files = list(get_files('doc', '*'))
ninja.build(site, 'site',
        inputs = 'doc',
        implicit = site_files)

ninja.build('docs', 'phony',
        inputs = site)

ninja.default('header')

