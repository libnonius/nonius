class Toolchain:
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
        return ['/W3', '/WX']

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

