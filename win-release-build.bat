md build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -G "Visual Studio 14 2015" ..
devenv openrhythm.sln /build
cd ..
pause
