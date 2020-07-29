if [ ! -d build ]; then
    mkdir build
fi

if [ $# -ne 1 ]; then
    echo "[!] Invalid number of arguments: $#"
    echo "    Required: 1"
    echo "[*] Try running: "
    echo "./compile.sh <ARCHITECTURE>"
    exit 0
fi

ARCH=$1
if [ $ARCH -ne 32 ] && [ $ARCH -ne 64 ]; then
    echo "Invalid architecture: $ARCH"
    echo "Chose either 32 bits (32) or 64 bits (64)"
    exit 0
fi

if [ ! command -v zenity &> /dev/null ]; then
    echo "Unable to find 'zenity', make sure it is installed"
    exit 0
fi

clang++ -g -m$ARCH -shared -fPIC sdl2-hook/main.cpp sdl2-hook/mem/mem.cpp -o build/libinject.so -ldl -pthread
exit 1