#define current_dtype double // should really be defined in config but whatever 
#define FILE_P 15; // precision of outputs

using namespace std; // heresy

#include "imports.h"
#include <sstream>
#include <unordered_map>
#include "io.h"
#include "init_conditions.h"
#include "integrator.h"
#include <chrono> 
// #include <easy/profiler.h>

// Main simulation
int main ( int argc, char *argv[] ){
	// EASY_PROFILER_ENABLE
	// EASY_FUNCTION(profiler::colors::Magenta);
	double wt;
	int dim = 3;
	Region sim_region;
	string out_folder;

	auto seed = chrono::system_clock::now().time_since_epoch().count();
	// seed = 1608785537296692664;
	seed = 1;
	cout<<"Random seed used: "<<seed<<endl;
	default_random_engine rands(seed);

	auto config = read_config("test.config");
	int n_threads = config.n_threads;
	int n = config.n_particles; 
	int step(0), file_n(0);
	omp_set_num_threads( n_threads );
	mass = 0.001;
	current_dtype dt(0.1), t(0.), totalE(0.), tmax(config.tmax);
	Method method = config.method;
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
	NodePool<OctreeNode> node_pool(10000000); // need a good way of knowing this compile time!
	vector<OctreeNode*> node_map(n);
	vector<OctreeNode*> node_list;
	cout<<"=========="<<endl;

	switch(method) {
		case direct:
			leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n);
			break;
		case barnes_hutt:
			leapfrog_init_step_strided_BH(	strided_pos, strided_vel, strided_force, dt, n, sim_region, node_pool);
			break;
		default:
			cout<<"Switch error"<<endl;
	}

	cout<<"Initial totalE: " + to_string(totalE)<<endl;
	write_state(strided_pos, to_string(step)+"_pos");
	while(t<tmax){

		switch(method) {
			case direct:
				leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, sim_region, file_n);
				out_folder = "output/data/data_direct/";
				break;
			case barnes_hutt:
				leapfrog_step_strided_BH(strided_pos, strided_vel, strided_force, dt, n, sim_region, file_n, node_pool);
				out_folder = "output/data/data_bh/";
				break;
			default:
				cout<<"Switch error"<<endl;
		}

		t += dt;
		step++;
		if(step%10==0){
			file_n++;
			write_flat_table(strided_pos, out_folder + to_string(file_n)+"_pos.dat", dim);
			write_flat_table(strided_vel, out_folder + to_string(file_n)+"_vel.dat", dim);
			write_flat_table(strided_force, out_folder + to_string(file_n)+"_force.dat", dim);

		}
		progress( t/tmax, totalE );
	}
	
	cout<<"\n"<<endl;
	double t_total = ( get_wall_time() - wt );
	cout<< "Total time: " <<setprecision(3) << t_total <<"s"<<endl;
	cout<< "FPS: "<< setprecision(2) << (double) step/t_total <<endl;
	// cout<<setprecision(15) <<strided_pos[0]<<"\t"<<strided_pos[10]<<endl;

	return 0;
}
