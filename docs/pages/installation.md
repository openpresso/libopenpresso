# Installation

This page describes supported ways to consume libopenpresso. 
Given that libopenpresso itself uses conan and cmake in its builds,
having one of these tools in your project will simplify things a lot.

Pick an option that best fits your project: 
- [Conan package manager](#install-with-conan)
- [CMake FetchContent](#install-cmake-fetchcontent)
- [Add sources as cmake subdirectory](#install-cmake-subdirectory)
- [Download binaries from GitHub Releases](#install-prebuilt-binaries)
- [Build from sources](#install-build-from-sources)

---

## Conan package (preferred) {#install-with-conan}
Use Conan package manager to install libopenpresso as a dependency
to your project. This will make sure that all subdependencies are
managed correctly and can be found by their consumers. Conan also provides
a convenient way to manage build toolchain via profiles and CMakeToolchain
generator, which is especially useful for cross-compiling.

1. Add openpresso official remote:
```bash
conan remote add openpresso-conan https://conan.cloudsmith.io/openpresso/@LIBOPENPRESSO_REPO_CHANNEL@
```

2. Add requires statement to your conan recipe:
- conanfile.py
```python
from conan import ConanFile

class ExampleRecipe(ConanFile):
    def requirements(self):
        self.requires("libopenpresso/@LIBOPENPRESSO_VERSION@")
```
- conanfile.txt
```ini
[requires]
libopenpresso/@LIBOPENPRESSO_VERSION@
```

Then in your cmake project simply write:
```cmake
find_package(libopenpresso)
target_link_libraries(myapp PRIVATE libopenpresso::libopenpresso)
```


> [!TIP]  
> For more information, including how to consume conan-installed packages in
> build systems other than cmake, see [conan documentation](https://docs.conan.io/2/tutorial/consuming_packages.html).

---

## CMake FetchContent {#install-cmake-fetchcontent}
For projects using CMake as build system without external package manager:

```cmake
include(FetchContent)
FetchContent_Declare(libopenpresso 
    URL https://github.com/openpresso/libopenpresso/archive/refs/heads/master.zip)
FetchContent_MakeAvailable(libopenpresso)

target_link_libraries(myapp PRIVATE libopenpresso::libopenpresso)
```

> [!TIP]  
> For more information see [cmake documentation](https://cmake.org/cmake/help/latest/module/FetchContent.html).

---

## CMake subdirectory {#install-cmake-subdirectory}
You can manually fetch sources from git repo or download archive from GitHub
and call `add_subdirectory` in your project CMakeLists.txt:

```bash
git clone https://github.com/openpresso/libopenpresso.git
```

```cmake
add_subdirectory(libopenpresso)
target_link_libraries(myapp PRIVATE libopenpresso::libopenpresso)
```

> [!TIP] 
> It's recommended to checkout on one of the [release tags](https://github.com/openpresso/libopenpresso/tags)
> to be sure you are using a stable version.

---

> [!WARNING]
> Use the following options carefully and only if your project doesn't use cmake as build system
> and cannot consume dependencies installed with conan. Build system will not be able
> to guarantee ABI compatibility and you should take care about matching build
> toolchains used for compiling libopenpresso and your project.

## Precompiled binaries {#install-prebuilt-binaries}
Libopenpresso provides prebuilt binaries for several popular target architectures.
You can download and extract binaries archive from 
[GitHub Releases](https://github.com/openpresso/libopenpresso/releases)
and manually add corresponding include path and link library to your project.

```bash
g++ my_awesome_espresso_app.cpp -I ../libopenpresso/include -L ../libopenpresso/lib -lopenpresso
```

> [!TIP]  
> To see what environment was used for build, check 
> [openpresso toolchains repo](https://github.com/openpresso/openpresso-toolchain).
> Libopenpresso CI workflow uses containers from this repo to build the binaries.

---

## Build from sources {#install-build-from-sources}
If you need to build libopenpresso for specific architecture, 
with additional compiler flags or just want to change something 
inside the library, you can build it from sources using 
[container with preconfigured build environment](https://github.com/orgs/openpresso/packages?repo_name=openpresso-toolchain) 
or configure environment by yourself.

> [!TIP]  
> For more information see [cross-compiling guide](crosscompiling.md).

```bash
git clone https://github.com/openpresso/libopenpresso.git
cd libopenpresso
docker run -it -v .:/workspace ghcr.io/openpresso/openpresso-toolchain-armv8 bash
cd /workspace
conan install . --build=missing
cmake . --preset conan-release
cmake --build . --preset conan-release
cmake --install ./build/Release --prefix ./out
exit
```

Now `out` directory should have the same structure as the precompiled binaries
from GitHub Releases and you can follow instructions from [previous section](#install-prebuilt-binaries).
