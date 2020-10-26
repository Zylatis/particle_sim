all:
	g++ -std=c++11 -O3 -o solver src/plummer_collision.cpp -fopenmp

clean:
	$(RM) solver

