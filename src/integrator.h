#include <assert.h> 
#include "barnes_hutt_objs.h"
#include "bh_test.h"
// #include "init_conditions.h"
// #include <easy/profiler.h>


bool particleInRegion2(double x, double y, double z, const Region &region){ // Should really call this from Barnes Hutt routine too 
	return (x<= region.xmax && x> region.xmin && y <= region.ymax && y > region.ymin && z <= region.zmax && z> region.zmin);
}

#define likely(x)       __builtin_expect(!!(x), 1)

double roundoff(double value, double prec)
{
  double pow_10 = pow(10.0, (double)prec);
  return round(value * pow_10) / pow_10;
}

// void get_diffs(vector<vector<double> > &vec1, vector<vector<double> > &vec2){


// }

// Calc force
void calc_force_strided(  const vector<current_dtype> &strided_pos_vec, vector<current_dtype > &strided_force, int n ){
	// EASY_FUNCTION();

	fill(strided_force.begin(), strided_force.end(),0.);

	#pragma omp parallel for 
	// Previously found that doing even supposedly threadsafe stuff + the symmetry trick caused problems so decided to bin the 1/2 factor in favour of parallelism
	for(int i = 0; i<n;i++){
		for(int j = 0; j<n; j++){
			if(i!=j){
				array<double,3> drvec = {0., 0., 0.};
				for(int k = 0; k<3;k++){
					drvec[k] = strided_pos_vec[3*j+k] - strided_pos_vec[3*i+k];
				}
				double rmag = sqrt(drvec[0]*drvec[0] + drvec[1]*drvec[1] + drvec[2]*drvec[2]);

				for(int k = 0; k<3;k++){
					double val = G*mass*drvec[k]/(eps+rmag*rmag*rmag);
					strided_force[3*i+k] += val;
					
				}
			}
		}
	}	
}

void barnes_hutt_force_step(const vector< current_dtype > &strided_pos, vector< current_dtype > &strided_force, int n, const Region &sim_region, NodePool<OctreeNode> &node_pool){
	// EASY_FUNCTION();
	fill(strided_force.begin(), strided_force.end(),0.);

	vector<OctreeNode*> node_map(n);
	vector<OctreeNode*> node_list;
	// cout<<node_pool.get()<<endl;

	OctreeNode* root_node = new (node_pool.get()) OctreeNode(sim_region.xmin, sim_region.xmax, sim_region.ymin, sim_region.ymax, sim_region.zmin, sim_region.zmax);

	node_list.push_back(root_node);
	for(int i = 0;i<n;i++){
		root_node->addParticle(i, strided_pos, node_map, node_list, node_pool);		

	}

	#pragma omp parallel for 
	for(int i = 0; i<n;i++){
		// TODO: this might be really dumb maybe we should make use of the hashmap of node<->particle locs to do this?
		root_node->calcForce(i, strided_pos, strided_force);	
	}

	exit(0);
	
	node_pool.reset();
	// delete root_node;
}

void leapfrog_step_strided( vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n, Region &sim_region, int file_n){

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] += strided_vel[3*i+k]*dt;

		}
	}

	calc_force_strided(strided_pos, strided_force, n);
	
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt;
			strided_vel[3*i+k] = roundoff(strided_vel[3*i+k],10);
		}
	}
}


// Initial t0 leapfrog step
void leapfrog_init_step_strided( const vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n){
	calc_force_strided(strided_pos, strided_force, n);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt*0.5;
		}
	}
}

// Debug purposes
void leapfrog_init_step_strided_BH(
		const vector< current_dtype > &strided_pos,
		vector< current_dtype > &strided_vel,
		vector< current_dtype > &strided_force,
		current_dtype dt,
		int n,
		const Region &sim_region,
		 NodePool<OctreeNode> &node_pool
	){

	barnes_hutt_force_step(strided_pos, strided_force, n, sim_region, node_pool);
	// cout<<"X"<<endl;
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt*0.5;
		}
	}
}

// Debug purposes
void leapfrog_step_strided_BH( vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n, Region &sim_region, int file_n,
	 NodePool<OctreeNode> &node_pool
	){

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] += strided_vel[3*i+k]*dt;
		}
		if(!particleInRegion2(strided_pos[3*i],strided_pos[3*i+1],strided_pos[3*i+2], sim_region)){
				cout<<"ERROR"<<endl;
				cout<<i<<endl;
				exit(0);
			}
	}

	barnes_hutt_force_step(strided_pos, strided_force, n, sim_region, node_pool);
	// cout<<"update vel"<<endl;
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt;
			strided_vel[3*i+k] = roundoff(strided_vel[3*i+k],10);

		}
	}
}
