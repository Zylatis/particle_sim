
// Get delta - r
vector<double> diff_vec(  const vector<vector< double > >&pos_vec , const int &i, const int &j){
	
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
vector<vector< double > > force( const vector<vector< double > > &pos_vec, const int &n ){
	vector<vector< double > >  out(n, vector<double>(3,0));
	
	#pragma omp parallel for
	for(int i = 0; i< n; i++){
		// for(int j = i+1; j<n; j++){
		for(int j = 0; j<n; j++){
			if(i!=j){
				vector<double> drvec = diff_vec(pos_vec, j, i);				
				double rmag = get_mag(drvec);
				for(int k = 0; k<3;k++){
					out[i][k] += G*mass*drvec[k]/(eps+pow(rmag,3));
					// #pragma omp atomic
					// out[j][k] += -G*mass*drvec[k]/(eps+pow(rmag,3));
				}
			}
		}
	}
	
	return out;	
}


void leapfrog_step( vector<vector< double > > &pos, vector<vector< double > > &vel, const double &dt, const int &n){
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			pos[i][k] += vel[i][k]*dt;
		}
	}
	vector<vector< double> > accel = force(pos, n);
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			vel[i][k] += accel[i][k]*dt;
		}
	}
}

// Initial t0 leapfrog step
void leapfrog_init_step( const vector<vector<double> > &pos, vector< vector<double> > &vel, const double &dt, const int &n){
	vector<vector<double> > accel = force(pos, n);
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			vel[i][k] += accel[i][k]*dt/2.;
		}
	}
}