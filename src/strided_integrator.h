#include <assert.h> 
#define likely(x)       __builtin_expect(!!(x), 1)

// Calc force
void calc_force_strided( const vector<double> &strided_pos_vec, const vector<double > &strided_vel_vec, vector<double > &strided_force, int n, double &totalE, vector<vector<double > > &strided_force_threadcpy ){
	totalE = 0.;	
	
	// #pragma omp parallel for
	// for(int i = 0; i<n;i++){
	// 	for(int k = 0; k<3;k++){
	// 		strided_force_threadcpy[thread_id][3*i+k] = 0.;
	// 	}
	// }
	
	fill(strided_force.begin(), strided_force.end(),0.);

	#pragma omp parallel for //schedule(static)
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
					// if(i==0){
					// 	cout<<val<<"\t";
					// 	if(k==2){
					// 		cout<<strided_force[3*i+k]<<"\t"<<j<<endl;
					// 	}
					// }
					// strided_force_threadcpy[thread_id][3*i+k] += val;
					// strided_force_threadcpy[thread_id][3*j+k] -= val;
				}
			}
		}
	}	

	// #pragma omp parallel for
	// for ( int i = 0; i < 3*n; i++ ) {
		// strided_force[i] = 0.;
		// for(int tid = 0; tid<n_threads;tid++){
        	// strided_force[i] += strided_force_threadcpy[tid][i];
   	 	// }
	// }		
}

void leapfrog_step_strided( vector< double > &strided_pos, vector< double > &strided_vel, vector< double > &strided_force, double dt, int n, double &totalE,  vector<vector<double > > &strided_force_threadcpy){
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] += strided_vel[3*i+k]*dt;
		}
	}
	calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt;
		}
	}
}

// Initial t0 leapfrog step
void leapfrog_init_step_strided( const vector< double > &strided_pos, vector< double > &strided_vel, vector< double > &strided_force, double dt, int n, double &totalE,  vector<vector<double > > &strided_force_threadcpy){
	calc_force_strided(strided_pos, strided_vel, strided_force, n, totalE, strided_force_threadcpy);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			strided_vel[3*i+k] += strided_force[3*i+k]*dt*0.5;
		}
	}
}