#!/bin/bash
mkdir -p extern
cd extern

if [ -d soundio ]; then
    cd soundio
    git pull
    if [ ! $? -eq 0 ]; then
        echo "Git pull failed, but continuing…"
    fi
else
    git clone https://github.com/andrewrk/libsoundio soundio
    if [ ! $? -eq 0 ]; then
        echo "Git clone failed ! Please check you network connection."
        exit 1
    fi
    cd soundio
fi


cmake -DCMAKE_INSTALL_PREFIX=.      \
      -DBUILD_STATIC_LIBS=ON        \
      -DBUILD_DYNAMIC_LIBS=ON       \
      -DBUILD_EXAMPLE_PROGRAMS=OFF  \
      -DBUILD_TESTS=OFF             \
      .
make
make install
