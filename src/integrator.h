#include <assert.h> 
#define likely(x)       __builtin_expect(!!(x), 1)

// Get delta - r
vector<double> diff_vec(  const vector<vector< double > >&pos_vec , const int &i, const int &j){
	// Note that currently this is called inside a pragma parallel loop so no point parallelising here
	vector<double> out(3);
	for(int k = 0; k<3;k++){
		out[k] = pos_vec[i][k] - pos_vec[j][k];
	}

	return out;
}

// Get magnitude
double get_mag( const vector<double> &pos_vec ){
	return sqrt(pow(pos_vec[0],2.) + pow(pos_vec[1],2.) + pow(pos_vec[2],2.));
}

// Calc force
void calc_force( const vector<vector< double > > &pos_vec, const vector<vector< double > > &vel_vec, vector<vector< double > > &force, int n, double &totalE ){
	totalE = 0.;	
	
	#pragma omp parallel for
	for(int i = 0; i< n; i++){

		// Lazy zeroing 
		for(int k = 0; k<3;k++){
			fill(force[i].begin(), force[i].end(), 0.);
		}
		double vmag = get_mag(vel_vec[i]);

		#pragma omp atomic
		totalE += mass*vmag*vmag*0.5;

		for(int j = 0; j<n; j++){
			if(likely(i!=j)){
			// if(i!=j){
				vector<double> drvec = diff_vec(pos_vec, j, i);				
				double rmag = get_mag(drvec);
				
				for(int k = 0; k<3;k++){
					force[i][k] += G*mass*drvec[k]/(eps+rmag*rmag*rmag);
					// #pragma omp atomic 
					// out[j][k] += -G*mass*drvec[k]/(eps+pow(rmag,3));
				}

				#pragma omp atomic
				totalE += -G*mass*mass/rmag;
			}
		}
	}
	// return out;	
}

void leapfrog_step( vector<vector< double > > &pos, vector<vector< double > > &vel, vector< vector<double> > &force, double dt, int n, double &totalE){
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			pos[i][k] += vel[i][k]*dt;
		}
	}
	// vector<vector< double> > accel = force(pos, vel, n, totalE);
	calc_force(pos, vel, force, n, totalE);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			vel[i][k] += force[i][k]*dt;
		}
	}
}

// Initial t0 leapfrog step
void leapfrog_init_step( const vector<vector<double> > &pos, vector< vector<double> > &vel, vector< vector<double> > &force, double dt, int n, double &totalE){
	// vector<vector<double> > accel = force(pos, vel, n, totalE);
	calc_force(pos, vel, force, n, totalE);

	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			vel[i][k] += force[i][k]*dt/2.;
		}
	}
}