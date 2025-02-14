clear
# g++ -shared -fPIC libc_stat.cc init.cc ms_print.cc -o libmy_mem.so -ldl # -lunwind
g++ -shared -fPIC lib.c lib_cpp.cc concurrent_queue.c init.c ms_print.c -o libmy_mem.so -ldl -lunwind -lpthread -ljemalloc
clang++ -std=c++17 test.cc -L. -lmy_mem