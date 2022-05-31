# Build
A C++20 compiler is a base requirement.

## Clone
Use [Git](https://git-scm.com/) to clone the repository and its submodules.

```
> git clone https://github.com/jsmolka/drizzle
> cd drizzle
> git submodule update --init
```

## Build

### Windows
Build the Visual Studio solution. This can also be done on the command line.

```
> call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
> msbuild /target:drizzle /property:Configuration=Release drizzle.sln
```

### Linux and macOS
Build with [CMake](https://cmake.org/).

```
$ mkdir drizzle/build
$ cd drizzle/build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native" ..
$ make -j 4
```
