CXX = g++ -std=c++11 -Wall -O2
CLEAN = rm -rf

all:
	$(CXX) src/*.cpp -c
	$(CXX) main.o pattern.o decision.o -o lrg

clean:
	$(CLEAN) *.o lrg
