These instructions are quite sparse currently.

get the latest versions of the following:
* cmake
* freetype
* GLM

You will need a compiler that supports C++11
* Windows: Visual Studio 2013 Community Edition
* Linux: May be distro specific, but gcc, and make will be required atleast.
* Mac: Xcode

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
