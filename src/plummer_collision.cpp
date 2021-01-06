#define current_dtype double // should really be defined in config but whatever 
using namespace std; // heresy

#include <sstream>
#include <typeinfo>
#include <chrono> 
#include "imports.h"
#include "io.h"
#include "init_conditions.h"
#include "integrator.h"
// #include <easy/profiler.h>

// Main simulation
int main ( int argc, char *argv[] ){
	// EASY_PROFILER_ENABLE
	// EASY_FUNCTION(profiler::colors::Magenta);
	double wt;
	auto config = read_config("test.config");
	int step(0), file_n(0);
	int dim = 3; // TODO: move to config
	int n = config.n_particles;

	current_dtype dt(config.dt), t(0.), tmax(config.tmax);
	Method method = config.method;
	current_dtype region_size = 3000;

	Universe universe;
	init_universe(universe, config.n_particles, dim);
	mass = universe.mass; // TODO: sample from dist, probably in particular init function 

	Region sim_region;
	sim_region.xmax = region_size;
	sim_region.xmin = -region_size;
	sim_region.ymax = region_size;
	sim_region.ymin = -region_size;
	sim_region.zmax = region_size;
	sim_region.zmin = -region_size;

	string out_folder;

	omp_set_num_threads( config.n_threads );

	auto seed = chrono::system_clock::now().time_since_epoch().count();
	seed = 1;
	cout<<"Random seed used: "<<seed<<endl;
	default_random_engine rands(seed);

	// Lazy setup of cluster 1
	cout<<"Initalise:"<<endl;
	wt = get_wall_time();
	for(int i = 0; i<n/2; i++){	
		vector<vector<current_dtype> > temp = init( rands,{-10.,-10.,-10.}, {0.06,0.02,0.02}, sim_region);
		for(int k = 0; k<3;k++){
			universe.strided_pos[3*i+k] = temp[0][k];
			universe.strided_vel[3*i+k] = temp[1][k];
		}
	}
	// Lazy setup of cluster 2
	for(int i = n/2; i<n; i++){	
		vector<vector<current_dtype> > temp = init( rands,{10.,10.,10.}, {-.07,-0.01,-0.02}, sim_region);
		for(int k = 0; k<3;k++){
			universe.strided_pos[3*i+k] = temp[0][k];
			universe.strided_vel[3*i+k] = temp[1][k];
		}
	}
	NodePool<OctreeNode> node_pool(10000000); // need a good way of knowing this compile time!
	vector<OctreeNode*> node_map(n);
	vector<OctreeNode*> node_list;
	cout<<"=========="<<endl;

	switch(method) {
		case direct:
			leapfrog_init_step_strided(universe.strided_pos, universe.strided_vel, universe.strided_force, dt, n);
			break;
		case barnes_hutt:
			leapfrog_init_step_strided_BH(	universe.strided_pos, universe.strided_vel, universe.strided_force, dt, n, sim_region, node_pool);
			break;
		default:
			cout<<"Switch error"<<endl;
	}

	// cout<<"Initial totalE: " + to_string(totalE)<<endl;
	// write_state(strided_pos, to_string(step)+"_pos");
	while(t<tmax){
		switch(method) {
			case direct:
				leapfrog_step_strided(universe.strided_pos, universe.strided_vel, universe.strided_force, dt, n, sim_region, file_n);
				out_folder = "output/data/data_direct/";
				break;
			case barnes_hutt:
				leapfrog_step_strided_BH(universe, dt, sim_region, file_n, node_pool);
				// leapfrog_step_strided_BH(universe.strided_pos, universe.strided_vel, universe.strided_force, dt, n, sim_region, file_n, node_pool);
				out_folder = "output/data/data_bh/";
				break;
			default:
				cout<<"Switch error"<<endl;
		}

		t += dt;
		step++;
		if(step%10==0){
			file_n++;
			write_flat_table(universe.strided_pos, out_folder + to_string(file_n)+"_pos.dat", dim);
			write_flat_table(universe.strided_vel, out_folder + to_string(file_n)+"_vel.dat", dim);
			write_flat_table(universe.strided_force, out_folder + to_string(file_n)+"_force.dat", dim);
		}
		progress( t/tmax );
	}
	
	cout<<"\n"<<endl;
	double t_total = ( get_wall_time() - wt );
	cout<< "Total time: " <<setprecision(3) << t_total <<"s"<<endl;
	cout<< "FPS: "<< setprecision(2) << (double) step/t_total <<endl;
	cout<<setprecision(15) <<universe.strided_pos[0]<<"\t"<<universe.strided_pos[10]<<endl;

	return 0;
}
