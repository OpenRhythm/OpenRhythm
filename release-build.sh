#rm -rf build
mkdir build
cd ./build

if [ "$1" = "--noapp" ] || [ "$2" = "--noapp" ]
then
    export NO_APP="True"
elif [ "$1" = "--cores" ] || [ "$2" = "--cores" ]
then
	export MULCO_COMP="True"
else
	export MULCO_COMP="False"
fi
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release ..

if [ "$MULCO_COMP" = "True" ]
then
	make -j
else
	cmake --build .
fi
