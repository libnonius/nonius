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

class GccLikeToolchain:
    def compiler(self):
        return 'g++'
    def linker(self):
        return 'g++'

    def include(self, d):
        return '-I' + d;
    def dependency_include(self, d):
        return '-isystem' + d

    def define(self, d):
        return '-D' + d;

    def library(self, l):
        return '-l' + l;

    def common_cxx_flags(self):
        return ['-std=c++11', '-pthread']
    def cxx_flags(self):
        return ['-c'] + self.common_cxx_flags()
    def link_flags(self):
        return self.common_cxx_flags() + self.max_warnings()

    def debug_flags(self):
        return ['-g', '-Og']
    def optimisation_flags(self):
        return ['-O3']
    def compiler_lto_flags(self):
        return ['-flto']
    def linker_lto_flags(self):
        return ['-flto']

    def max_warnings(self):
        return ['-pedantic', '-Wall', '-Wextra', '-Werror']

    def compiler_output(self, file):
        return ['-o ' + file]
    def linker_output(self, file):
        return self.compiler_output(file)
    def executable_extension(self):
        return ''
    def dependencies_output(self, file):
        return ['-MMD', '-MF ' + file]
    def ninja_deps_style(self):
        return 'gcc'

class MsvcLikeToolchain:
    def compiler(self):
        return 'cl'
    def linker(self):
        return 'link'

    def include(self, d):
        return '/I' + d;
    def dependency_include(self, d):
        return self.include(d)

    def define(self, d):
        return '/D' + d;

    def library(self, l):
        return l;

    def cxx_flags(self):
        return ['/nologo', '/c', '/TP', '/EHsc']
    def link_flags(self):
        return ['/NOLOGO']

    def debug_flags(self):
        return ['/MDd', '/ZI', '/Od', '/RTC1']
    def optimisation_flags(self):
        return ['/MD', '/O2', '/Oy-', '/Oi']
    def compiler_lto_flags(self):
        return ['/GL']
    def linker_lto_flags(self):
        return []

    def max_warnings(self):
        return ['/W3', '/WX',
                '/wd4244', '/wd4267'] # there's too much noise

    def compiler_output(self, file):
        return ['/Fo' + file]
    def linker_output(self, file):
        return ['/OUT:' + file]
    def executable_extension(self):
        return '.exe'
    def dependencies_output(self, file):
        return ['/showIncludes']
    def ninja_deps_style(self):
        return 'msvc'

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
parser.add_argument('--cxx', default=None, metavar='executable', help='compiler name to use')
parser.add_argument('--msvc', action='store_true', help='use the MSVC++ toolchain')
parser.add_argument('--boost-dir', default=None, metavar='path', help='path of boost folder (i.e. the folder with include/ and lib/ subfolders)')
parser.add_argument('--no-lto', action='store_true', help='do not perform link-time optimisation')
args = parser.parse_args()

tools = MsvcLikeToolchain() if args.msvc else GccLikeToolchain()
compiler = args.cxx if args.cxx else tools.compiler()
linker = args.cxx if args.cxx else tools.linker()

# --- variables

dependencies = ['catch', 'wheels']
include_flags = flags([tools.include('include')],
                      (tools.dependency_include(os.path.join('deps', d, 'include')) for d in dependencies),
                      [tools.dependency_include('deps/cpptemplate')])
if(args.boost_dir):
    include_flags += ' ' + tools.dependency_include(args.boost_dir)
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
        description = 'LINK $in')

ninja.rule('stringize',
        command = ' '.join(['python', stringize_tool, '$in', '$out']),
        description = 'STRINGIZE $in')

ninja.rule('header',
        command = ' '.join(['python', single_header_tool, '$in', '$out']),
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

ninja.default('examples')

