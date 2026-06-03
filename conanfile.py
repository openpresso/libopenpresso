from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.build import check_min_cppstd
from conan.tools.files import copy
import re

class Libopenpresso(ConanFile):
    name = "libopenpresso"
    homepage = "https://openpresso.org"
    url = "https://github.com/openpresso/libopenpresso.git"
    license = "GPL-3.0-or-later"
    package_type = "static-library"
    settings = "os", "arch", "compiler", "build_type"
    exports_sources = "src/*", "include/*", "cmake/*", "CMakeLists.txt"
    options = { 
        "with_docs": [True, False], 
        "with_clang_tools": [True, False],
        "min_log_level": ["trace", "debug", "info", "warn", "err", "critical", "off"]
    }
    default_options = { 
        "with_docs": False, 
        "with_clang_tools": False,
        "min_log_level": "info"
    }
    
    def set_version(self):
        if not self.version:
            self.version = "0.0.0-unknown"
                
    def validate(self):
        check_min_cppstd(self, "23")
        if not self.settings.os == "Linux":
            raise ConanInvalidConfiguration("Only Linux is supported")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.30]")
        if self.options.with_docs:
            self.tool_requires("doxygen/[>=1.16.0 <1.17.0]")

    def requirements(self):
        self.requires("boost/[>=1.90.0]", options = { "header_only": True }, visible = False)
        if self.options.min_log_level != "off":
            self.requires("spdlog/[>=1.17.0 <2.0]")
        
    def generate(self):
        major, minor, patch = self.__version_components()
        tc = CMakeToolchain(self)

        if self.options.with_clang_tools:
            self.__intall_clang_tools(tc)

        tc.cache_variables["LIBOPENPRESSO_VERSION"] = self.version
        tc.cache_variables["LIBOPENPRESSO_VERSION_MAJOR"] = str(major)
        tc.cache_variables["LIBOPENPRESSO_VERSION_MINOR"] = str(minor)
        tc.cache_variables["LIBOPENPRESSO_VERSION_PATCH"] = str(patch)
        tc.cache_variables["LIBOPENPRESSO_REPO_CHANNEL"] = self.__repo_channel()
        tc.cache_variables["LIBOPENPRESSO_MIN_LOG_LEVEL"] = self.options.min_log_level
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_id(self):
        self.info.options.rm_safe("with_docs")
        self.info.options.rm_safe("with_clang_tools")

    def package_info(self):
        self.cpp_info.libs = ["openpresso"]
        self.cpp_info.system_libs = ["pthread"]
        self.cpp_info.requires = ["spdlog::spdlog"]

    def deploy(self):
        copy(self, "*", excludes=[ "conaninfo.txt", "conanmanifest.txt" ], src=self.package_folder, dst=self.deploy_folder)

    def __repo_channel(self) -> str:
        release_pattern = r'\d+\.\d+\.\d+(?:-rc\d+)?'
        canary_pattern = r'\d+\.\d+\.\d+-\d+-g[0-9a-f]{7}'
        if re.fullmatch(release_pattern, self.version):
            return "stable"
        if re.fullmatch(canary_pattern, self.version):
            return "canary"
        return "testing"

    def __version_components(self):
        try:
            pattern = r'(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)'
            match = re.match(pattern, self.version)
            return int(match.group("major")), int(match.group("minor")), int(match.group("patch"))
        except Exception:
            return 0, 0, 0

    def __intall_clang_tools(self, cmtc):
        try:
            from conan.tools.system import PyEnv
        except ImportError:
            from conan.tools.system import PipEnv as PyEnv
        venv = PyEnv(self)
        venv.install(["clang-tidy", "clang-format"])
        venv.generate()
        bin_dir = getattr(venv, "bin_path", None) or getattr(venv, "bin_dir", None)
        cmtc.cache_variables["CMAKE_PROGRAM_PATH"] = bin_dir

