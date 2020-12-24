#include <assert.h> 
#define likely(x)       __builtin_expect(!!(x), 1)

// Calc force
void calc_force_strided(  const vector<current_dtype> &strided_pos_vec, const vector<current_dtype > &strided_vel_vec, vector<current_dtype > &strided_force, int n ){
	
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
void leapfrog_step_strided( vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n){
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] += strided_vel[3*i+k]*dt;
		}
	}
	calc_force_strided(strided_pos, strided_vel, strided_force, n);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt;
		}
	}
}

// Initial t0 leapfrog step
void leapfrog_init_step_strided( const vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n){
	calc_force_strided(strided_pos, strided_vel, strided_force, n);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt*0.5;
		}
	}
}