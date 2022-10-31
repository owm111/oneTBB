# Config varaibles

#CXX = dpcpp
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pipe -O3 -Wall # -march=native
CPPFLAGS = -I/opt/intel/oneapi/tbb/latest/include
LDFLAGS = -L/opt/intel/oneapi/tbb/latest/lib/intel64/gcc4.8
LDLIBS = -lc -lm -lstdc++ -ltbb

progs = recursive-fib

all: $(progs)

clean:
	rm -f $(progs) *.o

.PHONY: all clean

recursive-fib: recursive-fib.o observers.o load_balance.o
