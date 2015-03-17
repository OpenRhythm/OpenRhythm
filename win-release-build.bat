md build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -G "Visual Studio 12 2013" ..
devenv musicgame.sln /build
pause