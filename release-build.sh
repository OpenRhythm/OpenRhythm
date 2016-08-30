# If CMakeCache.txt is present, it has to be deleted to use the new build root
rm CMakeCache.txt -r
mkdir build/ -p
cd    build/

export NO_APP="False"

# Read args
while [[ $1 ]]; do
    case "$1" in
        --noapp)
            export NO_APP="True"
            shift
            ;;
        --cores)
            MAKE_ARGS+=" -j "
            shift
            ;;
        --verbose|-v)
            MAKE_ARGS+=" VERBOSE=1 "
            shift
            ;;
        *)
            shift
            ;;
    esac
done

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release ..

make $MAKE_ARGS
