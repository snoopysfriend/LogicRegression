CXX = g++ -std=c++11 -Wall -O2
CLEAN = rm -rf

all:
	$(CXX) src/*.cpp -c
	$(CXX) -D TREE main.o pattern.o src/decision.cpp -o lrg-tree
	$(CXX) -D FOREST main.o pattern.o src/decision.cpp -o lrg-forest
	$(CXX) -D SFLIP src/main.cpp pattern.o src/decision.cpp -o lrg-sflip
	$(CXX) test.o pattern.o decision.o -o test 
	$(CXX) abc_opt.o pattern.o decision.o -o abc_opt 
clean:
	$(CLEAN) *.o lrg
