if [ $# -ne 1 ]; then
    echo "Error: incorrect number of arguments"
    exit 1
fi

clang++ -c -fpass-plugin=build/libPass.so $1 -O0 -o log/tmp.o
ar rcs log/libres.a log/tmp.o build/CMakeFiles/log_func.dir/source/pass/log_func.c.o
