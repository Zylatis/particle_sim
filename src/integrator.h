
// Calc force
void calc_force_strided( const vector<double> &strided_pos_vec, const vector<double > &strided_vel_vec, vector<double > &strided_force, int n, double &totalE, vector<vector<double > > &strided_force_threadcpy);

void leapfrog_step_strided( vector< double > &strided_pos, vector< double > &strided_vel, vector< double > &strided_force, double dt, int n, double &totalE,  vector<vector<double > > &strided_force_threadcpy);

// Initial t0 leapfrog step
void leapfrog_init_step_strided( const vector< double > &strided_pos, vector< double > &strided_vel, vector< double > &strided_force, double dt, int n, double &totalE,  vector<vector<double > > &strided_force_threadcpy);