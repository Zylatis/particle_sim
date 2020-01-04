using namespace std;
int thread_id, n_threads;
#pragma omp threadprivate( thread_id )

#include "imports.h"
#include "io.h"
#include "math_objs.h"
#include "init_conditions.h"
#include "integrator.h"
#include "strided_integrator.h"

#include "boost/program_options.hpp" 
#include "barnes_hutt_objs.h"
namespace po = boost::program_options;

int FILE_P = 15; // precision of outputs
int n;

po::variables_map process_pars(int argc, char *argv[]){
	po::options_description desc("Options"); 
	desc.add_options() 
	  ("help", "Print help messages") 
	  ("npar", po::value<int>(), "number of particles") 
	  ("nproc", po::value<int>(),"number of processes")
	  ("tmax", po::value<int>(), "max time") ; 

   
	vector<string> req_pars = {"nproc", "npar" , "tmax"};

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("help")) {
		cout << desc << "\n";
		exit(0);
	}

	vector<string> missing_pars = {};
	for(auto p : req_pars){
		if(vm.count(p) == 0){
			missing_pars.push_back(p);
		}
	}

	if(missing_pars.size()!=0){
		string failed = "";
		for(int i = 0; i<missing_pars.size();i++){
			if(i==0){
				failed += missing_pars[i];
			} else {
				failed += ", " + missing_pars[i];
			}
		}

		cout<<"Was not given following required pars: " + failed<<endl;
		exit(0);
	}

	return vm;
}

// Main simulation
int main ( int argc, char *argv[] ){
	double wt(0);
	po::variables_map vm = process_pars(argc, argv);
	empty_folder("output/data/",".dat");
	empty_folder("output/plots/",".png");
	
	n_threads = vm["nproc"].as<int>();
	omp_set_num_threads( n_threads );
	#pragma omp parallel
	{
		thread_id = omp_get_thread_num();
	}

	srand(1);
	default_random_engine rands;
	n = vm["npar"].as<int>();
	mass = 1./n;
	int step(0), file_n(0);
	double dt(0.1), t(0.), totalE(0.), tmax( vm["tmax"].as<int>() );

	vector<double> strided_pos(3*n,0.), strided_vel(3*n,0.), strided_force(3*n,0.);
	vector<vector<double> > strided_force_threadcpy(n_threads, vector<double>(3*n,0));
	vector<OctreeNode*> node_map(n);
	vector<OctreeNode*> node_list;
	double xmin(-20), xmax(20), ymin(-20), ymax(20), zmin(-20), zmax(20); // TODO add assert that all particles produced inside box

	// Lazy setup of cluster 1
	for(int i = 0; i<n/2; i++){	
		vector<vector<double> > temp = init( rands,{-10.,-10.,-10.},{0.06,0.02,0.02});
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// Lazy setup of cluster 2
	for(int i = n/2; i<n; i++){	
		vector<vector<double> > temp = init( rands,{10.,10.,10.},{-.07,-0.01,-0.02});
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	strided_pos[0] = -15;
	strided_pos[1] = -15;
	strided_pos[2] = -15;

	strided_pos[3] = -1;
	strided_pos[4] = -1;
	strided_pos[5] = -1;

	wt = get_wall_time();
	OctreeNode* root_node = new OctreeNode(xmin, xmax, ymin, ymax, zmin, zmax);
	node_list.push_back(root_node);
	for(int i = 0;i<n;i++){
		// cout<<strided_pos[3*i+0]<<"\t"<<strided_pos[3*i+1]<<"\t"<<strided_pos[3*i+2]<<endl;
		root_node->addParticle(i, strided_pos, node_map, node_list);		
	}
	// cout<<get_wall_time()-wt<<endl;
	wt = get_wall_time();
	// cout<<root_node<<endl;
	// for(auto node: root_node->children){
	// 	node->printBoundaries();

	// }
	// for(int i=0; i<n; i++){
	// 	cout<<i<<"\t"<<strided_pos[3*i+0]<<"\t"<<strided_pos[3*i+1]<<"\t"<<strided_pos[3*i+2]<<"\t"<<node_map[i]<<endl;
	// }
	
	#pragma omp parallel for 
	for(int i = 0; i<n;i++){
		root_node->calcForce(i, strided_pos, strided_force);	
	}
	// cout<<get_wall_time()-wt<<endl;
	cout<<setprecision(15)<<strided_force[0]<<"\t"<<strided_force[1]<<"\t"<<strided_force[2]<<endl;
	cout<<setprecision(15)<<strided_force[3]<<"\t"<<strided_force[4]<<"\t"<<strided_force[5]<<endl;
	cout<<endl;
	strided_force[0] = 0.;
	strided_force[1] = 0.;
	strided_force[2] = 0.;

	strided_force[3] = 0.;
	strided_force[4] = 0.;
	strided_force[5] = 0.;

	wt = get_wall_time();
	calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy);
	// cout<<get_wall_time()-wt<<endl;
	cout<<setprecision(15)<<strided_force[0]<<"\t"<<strided_force[1]<<"\t"<<strided_force[2]<<endl;
	cout<<setprecision(15)<<strided_force[3]<<"\t"<<strided_force[4]<<"\t"<<strided_force[5]<<endl;

	exit(0);

	// Init leapfrom half step
	leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
	
	cout<<"Initial totalE: " + to_string(totalE)<<endl;
	double perc = 0;
	write_state(strided_pos, to_string(file_n)+"_pos");
	wt = get_wall_time();
	while(t<tmax){
	
		leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
		if(step%10==0){
			write_state(strided_pos, to_string(file_n)+"_pos");
			file_n++;
		}
		t += dt;
		step++;
		progress( t/tmax, totalE );
	}

	cout<<"\n"<<endl;
	cout<< "Total time: " <<setprecision(3) <<( get_wall_time() - wt )<<"s"<<endl;
	return 0;
}
