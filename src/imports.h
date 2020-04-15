#include <omp.h> //for parallelization
#include <iostream>                                                             
#include <fstream>    
#include <vector>
#include <math.h>
#include <sys/time.h>
#include <iomanip>
#include <random>
#include <array>

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
current_dtype mass, G(1.);
current_dtype eps = 0.01;
