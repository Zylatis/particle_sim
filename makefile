all:
	g++ src/plummer_collision.cpp -o plummer_collision -std=c++11 src/include/glad.c -I/home/graeme/coding_local/particle_sim/cpp/src/include/ -lGL -lglfw -Ofast -std=c++11 -mtune=native -march=native -fopenmp
clean:
	$(RM) plummer_collission
