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
#include "barnes_hutt_objs.h"

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include "include/glad.h"
// #include <GLFW/glfw3.h>


// Main simulation
int main ( int argc, char *argv[] ){
	auto config = read_config("test.config");
	n_threads = config.n_threads;
	omp_set_num_threads( n_threads );

	// initialise threads with ids
	#pragma omp parallel
	{
		thread_id = omp_get_thread_num();
	}

	srand(1);
	default_random_engine rands;
	int n = config.n_particles; 
	mass = 1./n;
	int step(0), file_n(0);
	current_dtype dt(0.1f), t(0.), totalE(0.), tmax(config.tmax);//tmax( vm["tmax"].as<int>() );

	vector<current_dtype> strided_pos(3*n,0.), strided_vel(3*n,0.), strided_force(3*n,0.);
	vector<vector<current_dtype> > strided_force_threadcpy(n_threads, vector<current_dtype>(3*n,0.));
	vector<current_dtype>  strided_dt_threadcpy(n_threads*3, 0.);
	double xmin(-20), xmax(20), ymin(-20), ymax(20), zmin(-20), zmax(20); // TODO add assert that all particles produced inside box


	vector<OctreeNode*> node_map(n);
	vector<OctreeNode*> node_list;

	// Lazy setup of cluster 1
	cout<<"Initalise:"<<endl;
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


	// Initialise BH
	OctreeNode* root_node = new OctreeNode(xmin, xmax, ymin, ymax, zmin, zmax);
	node_list.push_back(root_node);
	for(int i = 0;i<n;i++){
		// cout<<strided_pos[3*i+0]<<"\t"<<strided_pos[3*i+1]<<"\t"<<strided_pos[3*i+2]<<endl;
		root_node->addParticle(i, strided_pos, node_map, node_list);		
	}



	// begin test
	#pragma omp parallel for 
	for(int i = 0; i<n;i++){
		root_node->calcForce(i, strided_pos, strided_force);	
	}
	// cout<<get_wall_time()-wt<<endl;
    cout<<"-----------"<<endl;
	cout<<setprecision(15)<<strided_force[0]<<"\t"<<strided_force[1]<<"\t"<<strided_force[2]<<endl;
	cout<<setprecision(15)<<strided_force[3]<<"\t"<<strided_force[4]<<"\t"<<strided_force[5]<<endl;
	cout<<endl;

	fill(strided_force.begin(),strided_force.end(),0.);


	calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy, strided_dt_threadcpy);
    cout<<"-----------"<<endl;
	cout<<setprecision(15)<<strided_force[0]<<"\t"<<strided_force[1]<<"\t"<<strided_force[2]<<endl;
	cout<<setprecision(15)<<strided_force[3]<<"\t"<<strided_force[4]<<"\t"<<strided_force[5]<<endl;


	exit(0);
	// Init leapfrom half step
	// leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy, strided_dt_threadcpy) ;

	
	// cout<<"Initial totalE: " + to_string(totalE)<<endl;
	// write_state(strided_pos, to_string(file_n)+"_pos");
	// double wt = get_wall_time();
	// while(t<tmax){
	
	// 	leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy, strided_dt_threadcpy) ;
	// 	// if(step%10==0){
	// 	// 	write_state(strided_pos, to_string(file_n)+"_pos");
	// 	// 	file_n++;
	// 	// }
	// 	t += dt;
	// 	step++;
	// 	progress( t/tmax, totalE );
	// }

	// GLFWwindow* window;
  // if (!glfwInit())
  //   exit(EXIT_FAILURE);
  
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  // window = glfwCreateWindow(640, 480, "Look mah!", NULL, NULL);
	// cout<<"\n"<<endl;
	// double t_total = ( get_wall_time() - wt );
	// cout<< "Total time: " <<setprecision(3) << t_total <<"s"<<endl;
	// cout<< "FPS: "<< setprecision(2) << (double) step/t_total <<endl;
	// cout<<setprecision(15) <<strided_pos[0]<<"\t"<<strided_pos[10]<<endl;
	return 0;
}
