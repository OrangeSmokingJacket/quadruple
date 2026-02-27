from conan import tools, ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.build import check_min_cppstd
from conan.errors import ConanInvalidConfiguration
class OtterbrixConan(ConanFile):
    name = "quadruple"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"

    def configure(self):
        self.requires("catch2/3.13.0@")

    def config_options(self):
        if self.settings.get_safe("compiler.cppstd") is None:
            self.settings.cppstd = 20

    def validate(self):
        pass

    def layout(self):
        cmake_layout(self)

    def imports(self):
        self.copy("*.so*", dst="build_tools", src="lib")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_CXX_STANDARD"] = "20"
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()