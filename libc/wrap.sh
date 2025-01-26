# g++ -c wrapper.c -std=c++17
g++ -std=c++17 -Wl,--script=wrap.ld wrapper.c -o test_program