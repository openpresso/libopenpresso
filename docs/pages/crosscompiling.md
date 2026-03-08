# Cross-Compiling

Since Libopenpresso mainly targets embedded systems and single-board computers with limited
computing power, it could be useful to cross-compile it for the target architecture on more
powerful host device.  
This guide also can be used to compile applications that use libopenpresso as a dependency. It's
even better to have libopenpresso sources included into your project build tree and built final application
according to this guide.  
We will demonstrate setup and build process for armv8 target as an example. To see what changes for
different architectures check [openpresso toolchains repo](https://github.com/openpresso/openpresso-toolchain)
which sets up build environment in container. You will find argument sets that are passed to build
a container image for different target architectures in github workflow file.

## Build environment for cross compilation

To be able to build Libopenpresso for target architecture you will need:
- **Conan package manager**: recommended to manage build toolchain profiles, may become strongly required
if libopenpresso will depend on other libraries in future. Conan is written with python, here is a list of
packages needed to enable all conan features.
```bash
apt-get install python3 python3-dev python3-pip python3-venv pkg-config
```
If you're building isolated environment like Docker image, you can skip creation of python venv and install
conan system wide:
```bash
pip install conan --break-system-packages
```
For general case better to use python virtual environment (do this in your project root folder):
```bash
python -m venv .venv
source .venv/bin/activate
pip install conan
```
- **CMake**: Libopenpresso uses CMake as build system (meta build system, yes). We will also install
`ninja` that will be used by CMake to actually run builds, but you can use `make` instead if you want.
`Git` is also recommended in case we want to use CMake FetchContent feature.
```bash
apt-get install cmake ninja-build git
```
- **Cross compiler**: build toolchain for target architecture: GCC or CLang (not tested) + libc
```bash
apt-get install build-essential gcc-aarch64-linux-gnu g++-aarch64-linux-gnu libc6-dev-arm64-cross
```
- **Linux headers for userspace development**: libopenpreso uses linux userspace API for GPIO, I2C and SPI
communications.
```bash
apt-get install linux-libc-dev-arm64-cross
```

## Creation of build profiles

As was mentioned above, we will use conan profiles to separate settings for local builds
and cross compiling.
To create build profile for local builds, you can simply call
```bash
conan profile detect
```
and verify the result by calling
```bash
cat ~/.conan2/profiles/default
```
Use it as a starting point to create your own profiles.
> [!TIP]  
> For full list of parameters that can be configured in profile see 
> [conan profiles reference](https://docs.conan.io/2/reference/config_files/profiles.html).

Now let's create another profile with a name `target` that will use our cross compiling toolchain.
```bash
echo "[settings]
arch=armv8
build_type=Release
compiler=gcc
compiler.cppstd=23
compiler.libcxx=libstdc++11
compiler.version=14
os=Linux
[buildenv]
CC=aarch64-linux-gnu-gcc
CXX=aarch64-linux-gnu-g++
LD=aarch64-linux-gnu-ld" > ~/.conan2/profiles/target
```

In conan naming convention profiles for local builds referred as build profiles, and
for target devices as host profiles. We can set our newly created profile as a default
host profile by adding it in global conan config. By doing this we won't need to specify
profiles explicitly when calling conan cli commands.

```bash
echo "core:default_profile=target" >> ~/.conan2/global.conf
echo "core:default_build_profile=default" >> ~/.conan2/global.conf
```

## Generation of CMake Toolchain from conan profile {#cross-compiling-conan-install-step}

Libopenpresso already has conanfile.py that asks conan to generate
`.cmake` file with properly configured toolchain settings. This will
point cmake to the compliler and linker we added to our profile.
If you build your own project with this method, it's
recommended to add `CMakeDeps` and `CMakeToolchain` generators to your
conanfile, and use `cmake_layout` to let conan create `CMakeUserPresets.json`
in the project root folder.  
After creation of your conanfile, call:
```bash
conan install . --build=missing
```
to install all the listed dependencies and
generate cmake toolchain files.
Check that `CMakeUserPresets.json` was created:

```bash
cat ./CMakeUserPresets.json
```

> [!TIP]  
> See methods **generate** and **layout** in **conanfile.py** in the libopenresso root folder for reference
> and [conan cmake tools reference](https://docs.conan.io/2/reference/tools/cmake.html).

## Run CMake config and build

At this point you can simply call
```bash
cmake . --preset conan-release
cmake --build . --preset conan-release
```
to build the project.

## Using prebuilt containers

As was mentioned earlier, openpresso project provides already preconfigured
[cross compiling environments](https://github.com/openpresso/openpresso-toolchain) 
as Docker images. You can just pull one of them from GitHub 
[container registry](https://github.com/orgs/openpresso/packages?repo_name=openpresso-toolchain) or 
build locally using [Dockerfile](https://github.com/openpresso/openpresso-toolchain/blob/main/Dockerfile) 
for any target you want. Simply bind mount your project root folder into the container, 
and start building from calling [conan install](#cross-compiling-conan-install-step).