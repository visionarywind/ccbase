clear
# g++ -shared -fPIC libc_stat.cc init.cc ms_print.cc -o libmy_mem.so -ldl # -lunwind
g++ -shared -fPIC lib.cc concurrent_queue.cc init.cc ms_print.cc -o libmy_mem.so -ldl -lunwind -lpthread -ljemalloc
clang++ -std=c++17 test.cc -L. -lmy_mem