class Toolchain:
    def compiler(self):
        return 'clang++'
    def linker(self):
        return 'clang++'

    def include(self, d):
        return '-I' + d;
    def dependency_include(self, d):
        return '-isystem' + d

    def define(self, d):
        return '-D' + d;

    def library(self, l):
        return '-l' + l;

    def common_cxx_flags(self):
        return ['-stdlib=libc++', '-std=c++11', '-pthread']
    def cxx_flags(self):
        return ['-c'] + self.common_cxx_flags()
    def link_flags(self):
        return self.common_cxx_flags() + self.max_warnings()

    def debug_flags(self):
        return ['-g', '-Og']
    def optimisation_flags(self):
        return ['-O3']
    def compiler_lto_flags(self):
        return []
    def linker_lto_flags(self):
        return []

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

