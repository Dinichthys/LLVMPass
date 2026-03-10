if [ $# -ne 1 ]; then
    echo "Error: incorrect number of arguments"
    exit 1
fi

cmake --build build
clang++ -c -fpass-plugin=build/libPass.so $1 -O0 -o log/tmp.o > log/tmp.dot
dot -Tsvg log/tmp.dot -o log/tmp.svg
