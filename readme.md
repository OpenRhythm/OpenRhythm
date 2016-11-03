# OpenRhythm

OpenRhythm currently depends on the following libs:
* CMake
* SDL2
* Freetype - I may switch to using stb_freetype haven't decided quite yet.
* GLM
* STB (included in extern/stb)
* Gettext

### Submodules
This project contains submodules. There are two ways to handle this.

When cloning:
```
git clone --recursive https://github.com/OpenRhythm/OpenRhythm
```
After clone:
```
git submodule init
git submodule update
```

## Compiling
You will need a compiler that supports C++14

#### Windows
You will need to get Visual Studio 2015 Community Edition (please expand)

Download the pre-built dependancies and make sure they are in the extern folder.

https://dl.dropboxusercontent.com/u/37405488/extern.zip
This are x64 release builds done with VS 2015

#### Unix: (MacOS/Linux)
GCC-4.9+ or Clang-3.4+ (MacOS: download Xcode Command Line Utils)

* Linux
    * Debian / Ubuntu / Linux Mint
    ```
    sudo apt-get install cmake sdl2-dev libfreetype-dev libglm-dev
    ```
    * ArchLinux / Manjaro / …
    ```
    sudo pacman -S cmake sdl2 freetype2 glm
    ```

Then build the code
```
./release-build.sh 
# or
./release-build.sh --verbose
```
