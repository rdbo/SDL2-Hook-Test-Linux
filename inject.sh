#/bin/bash

proc_name="$1"
proc_id=""
libpath="$(pwd)/build/libinject.so"

if [ $# -ne 1 ]; then
    echo "[!] Invalid number of arguments: $#"
    echo "    Required: 1"
    echo "[*] Try running: "
    echo "sudo ./inject.sh <PROCESS_NAME>"
    exit 0
fi

if [ "$EUID" -ne 0 ]; then
    echo "[!] Run as root"
    exit 0
fi

if [ ! command -v gdb &> /dev/null ]; then
    echo "[!] Unable to find GDB, make sure you have it installed"
    exit 0
fi

if ! proc_id=$(pgrep $proc_name) > /dev/null 2>&1; then
    echo "[!] The target process is not running"
    exit 0
fi

if [ ! -f $libpath ]; then
    echo "[!] Invalid shared library file"
fi

gdb -n -q -batch \
  -ex "attach $proc_id" \
  -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
  -ex "call \$dlopen(\"$libpath\", 1)" \
  -ex "detach" \
  -ex "quit" > /dev/null 2>&1

echo "[*] done"

exit 1
