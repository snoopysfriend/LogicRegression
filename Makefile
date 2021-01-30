CXX = g++ -std=c++11 -Wall -g
CLEAN = rm -rf

all:
	$(CXX) src/*.cpp -c
	$(CXX) -D TREE main.o pattern.o src/decision.cpp -o lrg-tree
	$(CXX) -D FOREST main.o pattern.o src/decision.cpp -o lrg-forest
	$(CXX) -D SFLIP src/main.cpp pattern.o src/decision.cpp -o lrg-sflip
	$(CXX) test.o pattern.o decision.o -o test

clean:
	$(CLEAN) *.o lrg
