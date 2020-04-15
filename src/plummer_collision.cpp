#define current_dtype float // should really be defined in config but whatever 
#define FILE_P 15; // precision of outputs

int thread_id, n_threads; // ugly globals

using namespace std; // heresy
#pragma omp threadprivate( thread_id )

#include "imports.h"
#include <sstream>
#include <unordered_map>
#include "io.h"
#include "math_objs.h"
#include "init_conditions.h"
#include "integrator.h"

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include "include/glad.h"
// #include <GLFW/glfw3.h>


// Main simulation
int main ( int argc, char *argv[] ){
	
	n_threads = 2;
	omp_set_num_threads( n_threads );

	// initialise threads with ids
	#pragma omp parallel
	{
		thread_id = omp_get_thread_num();
	}

	srand(1);
	default_random_engine rands;
	int n = 200; 
	mass = 1./n;
	int step(0), file_n(0);
	current_dtype dt(0.1f), t(0.), totalE(0.), tmax(200.);//tmax( vm["tmax"].as<int>() );

	vector<current_dtype> strided_pos(3*n,0.), strided_vel(3*n,0.), strided_force(3*n,0.);
	vector<vector<current_dtype> > strided_force_threadcpy(n_threads, vector<current_dtype>(3*n,0.));
	// vector<OctreeNode*> node_map(n);
	// current_dtype xmin(-16), xmax(16), ymin(-16), ymax(16), zmin(-16), zmax(16);

	// Lazy setup of cluster 1
	for(int i = 0; i<n/2; i++){	
		vector<vector<current_dtype> > temp = init( rands,{-10.,-10.,-10.},{0.06,0.02,0.02});
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// Lazy setup of cluster 2
	for(int i = n/2; i<n; i++){	
		vector<vector<current_dtype> > temp = init( rands,{10.,10.,10.},{-.07,-0.01,-0.02});
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// Init leapfrom half step
	leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
	
	cout<<"Initial totalE: " + to_string(totalE)<<endl;
	write_state(strided_pos, to_string(file_n)+"_pos");
	double wt = get_wall_time();
	while(t<tmax){
	
		leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
		// if(step%10==0){
		// 	write_state(strided_pos, to_string(file_n)+"_pos");
		// 	file_n++;
		// }
		t += dt;
		step++;
		progress( t/tmax, totalE );
	}

	cout<<"\n"<<endl;
	double t_total = ( get_wall_time() - wt );
	cout<< "Total time: " <<setprecision(3) << t_total <<"s"<<endl;
	cout<< "FPS: "<< setprecision(2) << (double) step/t_total <<endl;

	return 0;
}
