#include <omp.h> //for parallelization
#include <iostream>                                                             
#include <fstream>    
#include <ctime>
#include <numeric>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>
#include <set>
#include <sys/time.h>
#include <complex>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <random>
#include <future>
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
