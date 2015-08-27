# MusicGame

Just as a warning, I have yet to decide on a license for the code.

MusicGame currently depends on the following libs:
* CMake
* SDL2
* Freetype
* GLM
* TTVFS (included in extern/ttvfs)
* STB (included in extern/stb)
* PNG++ (optional default off, included in extern/png++)

### Submodules
This project also contains submodules. There are two ways to handle this.

When cloning:
```
git clone --recursive http://github.com/user/repo
```
After clone:
```
git submodule init
git submodule update
```

## Compiling
You will need a compiler that supports C++14

#### Windows
You will need to get Visual Studio 2013 Community Edition (please expand)

#### Unix: (MacOS/Linux)
GCC-4.9+ or Clang-3.4+ (MacOS: download Xcode Command Line Utils)

Note: Replace `<package-manager>` with the package manager for your system. (apt-get, yum, brew, port, etc.)

```
<package-manager> install cmake sdl2-dev libfreetype-dev libglm-dev
```

Then build the code
```
./release-build.sh
```
