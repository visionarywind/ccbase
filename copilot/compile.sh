clear
gcc -fPIC -shared -o libcustom_memory.so lib.c
clang++ test.cc
