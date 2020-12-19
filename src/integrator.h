#include <assert.h> 
#define likely(x)       __builtin_expect(!!(x), 1)

// Calc force
void calc_force_strided( const vector<current_dtype> &strided_pos_vec, const vector<current_dtype > &strided_vel_vec, vector<current_dtype > &strided_force, int n, current_dtype &totalE, vector<vector<current_dtype > > &strided_force_threadcpy, vector<current_dtype  > &strided_dt_threadcpy ){
	totalE = 0.;	
	
	#pragma omp parallel
	for(int i = 0; i<n;i++){
		for(int k = 0; k<3;k++){
			strided_force_threadcpy[thread_id][3*i+k] = 0.;
		}
	}
	
	#pragma omp parallel for schedule(static)
	for(int i = 0; i<n;i++){
		// for(int j = i+1; j<n; j++){ // Use symmetry of force
		for(int j = 0; j<n; j++){
			if(i!=j){
				array<current_dtype,3> drvec;
				for(int k = 0; k<3;k++){
					drvec[k] = strided_pos_vec[3*j+k] - strided_pos_vec[3*i+k];
					// NOTE: tried to be clever here and preallocate the dr vector to see if faster as fewer mem ops but possibly introduced false sharing?
					// strided_dt_threadcpy[3*thread_id+k] = strided_pos_vec[3*j+k] - strided_pos_vec[3*i+k];
				}
				current_dtype rmag = sqrt(drvec[0]*drvec[0] + drvec[1]*drvec[1] + drvec[2]*drvec[2]);
				for(int k = 0; k<3;k++){
					current_dtype val = G*mass*drvec[k]/(eps+rmag*rmag*rmag);

					strided_force_threadcpy[0][3*i+k] += val;
					// Use below if using symmetry of force aspect but seems to break when threaded
					// strided_force_threadcpy[0][3*j+k] -= val;
				}
			}
		}
	}	

	#pragma omp parallel for
	for ( int i = 0; i < 3*n; i++ ) {
		strided_force[i] = 0.;
		for(int tid = 0; tid<n_threads;tid++){
        	strided_force[i] += strided_force_threadcpy[tid][i];
   	 	}
	}		
}

void leapfrog_step_strided( vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n, current_dtype &totalE,  vector<vector<current_dtype > > &strided_force_threadcpy, vector<current_dtype  > &strided_dt_threadcpy){
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] += strided_vel[3*i+k]*dt;
		}
	}
	calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy, strided_dt_threadcpy);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt;
		}
	}
}

// Initial t0 leapfrog step
void leapfrog_init_step_strided( const vector< current_dtype > &strided_pos, vector< current_dtype > &strided_vel, vector< current_dtype > &strided_force, current_dtype dt, int n, current_dtype &totalE,  vector<vector<current_dtype > > &strided_force_threadcpy, vector<current_dtype > &strided_dt_threadcpy){
	calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy, strided_dt_threadcpy);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt*0.5;
		}
	}
}