if [ $# -ne 1 ]; then
    echo "Error: incorrect number of arguments"
    exit 1
fi

clang++ -c -fpass-plugin=build/libPass.so $1 -O0 -o log/tmp.o
clang++ log/tmp.o build/CMakeFiles/log_func.dir/source/pass/log_func.c.o -o log/tmp.out
./log/tmp.out > log/tmp.out.txt
./build/build_graph
dot -Tsvg log/tmp.dot -o log/tmp.svg
