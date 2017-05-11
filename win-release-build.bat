md build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -G "Visual Studio 15 2017 Win64" ..
devenv openrhythm.sln /build
cd ..
