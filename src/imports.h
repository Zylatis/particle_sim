#if defined(_WIN32)
    #define PLATFORM_NAME "windows" // Windows
    #include <Windows.h>
    // Function from 
    // https://stackoverflow.com/questions/17432502/how-can-i-measure-cpu-time-and-wall-clock-time-on-both-linux-windows
    double get_wall_time(){
        LARGE_INTEGER time,freq;
        if (!QueryPerformanceFrequency(&freq)){
            //  Handle error
            return 0;
        }
        if (!QueryPerformanceCounter(&time)){
            //  Handle error
            return 0;
        }
        return (double)time.QuadPart / freq.QuadPart;
    }

#elif defined(__linux__)
    #define PLATFORM_NAME "linux"
    #include <sys/time.h>
    double get_wall_time(){
        struct timeval time;
        if (gettimeofday(&time,NULL)){
            //  Handle error
            return 0;
        }
        return (double)time.tv_sec + (double)time.tv_usec * .000001;
    }
#endif

#include <omp.h> //for parallelization
#include <iostream>                                                             
#include <fstream>    
#include <vector>
#include <math.h>
#include <iomanip>
#include <random>
#include <array>


current_dtype mass, G(1.);
current_dtype eps = 0.01;

