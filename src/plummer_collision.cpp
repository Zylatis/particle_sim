#define current_dtype double // should really be defined in config but whatever 
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
#include <chrono> 

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include "include/glad.h"
// #include <GLFW/glfw3.h>


void test_destructor(int n, int n_runs, Region &sim_region, auto strided_pos, auto node_map, auto node_list){
	for(int x = 0;x<n_runs;x++){
		cout<<x<<endl;
		OctreeNode* root_node = new OctreeNode(sim_region.xmin, sim_region.xmax, sim_region.ymin, sim_region.ymax, sim_region.zmin, sim_region.zmax);
		for(int i = 0;i<n;i++){
			root_node->addParticle(i, strided_pos, node_map, node_list);		
		}
		delete root_node;
	}
}


// Main simulation
int main ( int argc, char *argv[] ){
	double wt;
	Region sim_region;

	auto seed = chrono::system_clock::now().time_since_epoch().count();
	seed = 1608785537296692664;
	cout<<"Random seed used: "<<seed<<endl;
	default_random_engine rands(seed);

	auto config = read_config("test.config");
	n_threads = config.n_threads;
	omp_set_num_threads( n_threads );

	// initialise threads with ids
	#pragma omp parallel
	{
		thread_id = omp_get_thread_num();
	}

	int n = config.n_particles; 
	mass = 1./n;
	int step(0), file_n(0);
	current_dtype dt(0.1f), t(0.), totalE(0.), tmax(config.tmax);;

	vector<current_dtype> strided_pos(3*n,0.), strided_vel(3*n,0.), strided_force(3*n,0.);

	// Lazy setup of cluster 1
	cout<<"Initalise:"<<endl;
	wt = get_wall_time();
	for(int i = 0; i<n/2; i++){	
		vector<vector<current_dtype> > temp = init( rands,{-10.,-10.,-10.}, {0.06,0.02,0.02}, sim_region);
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}
	// Lazy setup of cluster 2
	for(int i = n/2; i<n; i++){	
		vector<vector<current_dtype> > temp = init( rands,{10.,10.,10.}, {-.07,-0.01,-0.02}, sim_region);
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}
	// write_state(strided_pos, "debug_pos");
	// Initialise BH
	// test_destructor(n, 500, sim_region, strided_pos, node_map, node_list);

	// exit(0);

	// wt = get_wall_time();
	// barnes_hutt_force_step(strided_pos, strided_vel, strided_force, dt, n, sim_region);
	// cout<<(get_wall_time() - wt)<<endl;

 //    cout<<"-----------"<<endl;
	// cout<<setprecision(15)<<strided_force[0]<<"\t"<<strided_force[1]<<"\t"<<strided_force[2]<<endl;
	// cout<<setprecision(15)<<strided_force[3]<<"\t"<<strided_force[4]<<"\t"<<strided_force[5]<<endl;
	// cout<<endl;

	// fill(strided_force.begin(),strided_force.end(),0.);

	// wt = get_wall_time();
	// calc_force_strided(strided_pos, strided_vel, strided_force, n);
	// cout<<(get_wall_time() - wt)<<endl;

 //    cout<<"-----------"<<endl;
	// cout<<setprecision(15)<<strided_force[0]<<"\t"<<strided_force[1]<<"\t"<<strided_force[2]<<endl;
	// cout<<setprecision(15)<<strided_force[3]<<"\t"<<strided_force[4]<<"\t"<<strided_force[5]<<endl;


	// exit(0);
	// Init leapfrom half step
	leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n) ;

	
	cout<<"Initial totalE: " + to_string(totalE)<<endl;
	write_state(strided_pos, to_string(step)+"_pos");
	while(t<tmax){
	
		leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, sim_region) ;

		t += dt;
		step++;
		if(step%2==0){
			file_n++;
			write_state(strided_pos, to_string(file_n)+"_pos");
		}
		progress( t/tmax, totalE );
	}

	// GLFWwindow* window;
  // if (!glfwInit())
  //   exit(EXIT_FAILURE);
  
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  // window = glfwCreateWindow(640, 480, "Look mah!", NULL, NULL);
	cout<<"\n"<<endl;
	double t_total = ( get_wall_time() - wt );
	cout<< "Total time: " <<setprecision(3) << t_total <<"s"<<endl;
	cout<< "FPS: "<< setprecision(2) << (double) step/t_total <<endl;
	cout<<setprecision(15) <<strided_pos[0]<<"\t"<<strided_pos[10]<<endl;
	return 0;
}
