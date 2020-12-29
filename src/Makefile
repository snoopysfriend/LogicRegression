CXX = g++ -std=c++11 -Wall
CLEAN = rm -rf

all:
	$(CXX) -D LINUX *.cpp -c
	$(CXX) -D LINUX main.o pattern.o -o lrg

clean:
	$(CLEAN) *.o lrg
