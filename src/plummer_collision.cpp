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
	vector<double> blank(3, 0.);
	vector<vector< double > > pos(n,blank), vel(n,blank), force(n,blank);

	vector<double> strided_pos(3*n,0.), strided_vel(3*n,0.), strided_force(3*n,0.);
	vector<vector<double> > strided_force_threadcpy(n_threads, vector<double>(3*n,0));
	// Lazy setup of cluster 1
	// #pragma omp parallel for
	for(int i = 0; i<n/2; i++){	
		vector<vector<double> > temp = init( rands,{-10.,-10.,-10.},{0.06,0.02,0.02});
		pos[i] = temp[0];
		vel[i] = temp[1];

		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// Lazy setup of cluster 2
	// #pragma omp parallel for	
	for(int i = n/2; i<n; i++){	
		vector<vector<double> > temp = init( rands,{10.,10.,10.},{-.07,-0.01,-0.02});
		pos[i] = temp[0];
		vel[i] = temp[1];
	
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// calc_force(pos, vel, force, n, totalE);
	// calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy);
	// cout<<force[0][0]<<"\t"<<force[0][1]<<"\t"<<force[0][2]<<endl;
	// cout<<strided_force[3*0+0]<<"\t"<<strided_force[3*0+1]<<"\t"<<strided_force[3*0+2]<<endl;
	// cout<<endl;
	// cout<<force[3][0]<<"\t"<<force[3][1]<<"\t"<<force[3][2]<<endl;
	// cout<<strided_force[3*3+0]<<"\t"<<strided_force[3*3+1]<<"\t"<<strided_force[3*3+2]<<endl;
	// exit(0);
	// cout<<vel[0][0]<<"\t"<<vel[0][1]<<"\t"<<vel[0][2]<<endl;
	// cout<<strided_vel[3*0+0]<<"\t"<<strided_vel[3*0+1]<<"\t"<<strided_vel[3*0+2]<<endl;

	leapfrog_init_step(pos, vel, force, dt, n, totalE) ;
	leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
	
	
	cout<<"Initial totalE: " + to_string(totalE)<<endl;
	int xx = 12;
	double perc = 0;
	write_state(strided_pos, to_string(file_n)+"_pos");
	double wt = get_wall_time();
	while(t<tmax){
	
		leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
		leapfrog_step(pos, vel, force, dt, n, totalE);
		if(step%10==0){
			// write_state(strided_pos, to_string(file_n)+"_pos");
			// write_state(pos, to_string(file_n) + "_pos");
			file_n++;
		}
		t += dt;
		step++;
		progress( t/tmax, totalE );
	}

	cout<<"\n"<<endl;
	cout<< "Total time: " <<setprecision(3) <<( get_wall_time() - wt )<<"s"<<endl;
	cout<<force[100][0]<<"\t"<<force[100][1]<<"\t"<<force[100][2]<<endl;
	cout<<strided_force[3*100+0]<<"\t"<<strided_force[3*100+1]<<"\t"<<strided_force[3*100+2]<<endl;
	return 0;
}
