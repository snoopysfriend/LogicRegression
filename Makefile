CXX = g++ -std=c++11 -Wall -O2 -g
CLEAN = rm -rf

all:
	$(CXX) src/*.cpp -c
	$(CXX) main.o pattern.o decision.o -o lrg
	$(CXX) test.o pattern.o decision.o -o test

clean:
	$(CLEAN) *.o lrg
