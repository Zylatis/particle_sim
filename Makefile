all:
	g++ -std=c++14 -O3 -o solver src/plummer_collision.cpp -fopenmp

clean:
	$(RM) solver

