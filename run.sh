cmake --build build
clang++ -fpass-plugin=build/libPass.so examples/Stack/hash.cpp examples/Stack/stack.cpp examples/Stack/main.cpp  &> log/dot.dot
dot -Tsvg log/dot.dot -o log/dot.svg
