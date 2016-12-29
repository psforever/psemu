# psemu

A Planetside 1 server emulator. Based on the [PSForever](https://github.com/psforever) project.

## Building

Clone this repo.

#### Grab submodule dependencies:
```
cd psemu
git submodule update --init
```

#### Use [CMake](https://cmake.org/download):

**Either GUI:**

1. Source path = full path to psemu

2. Build path = full path to psemu/build

3. [Configure]

4. [Generate]

**Or command line:**
```
cd psemu
mkdir build
cd build
cmake ..
```

#### Use [Visual Studio](https://www.visualstudio.com/vs/cplusplus):
Build using the CMake-generated solution: psemu/build/psemu.sln
