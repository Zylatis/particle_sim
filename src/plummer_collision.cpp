using namespace std;

#include "imports.h"
#include "global_vars.h"
#include "math_objs.h"
#include "state_class.h"
#include "integrators.h"
//~ #include "io.h"


vector<vector<double> > plummer_init(  default_random_engine &rands, const vector<double> &centre, const vector<double> &centre_V ){
	
	vector<vector< double> > data = {vector<double>(3,0), vector<double>(3,0)};

	uniform_real_distribution<double> r_dist(0.,1.), unit_circle(-1,1), phi_dist(0,2.*3.14159), x_dist(0.,1.), y_dist(0.,0.1);
	double theta, r, phi, V;

	theta = acos(unit_circle(rands));
	phi = phi_dist(rands);
	r = 1./(sqrt(pow(r_dist(rands),-2./3.)-1.));
	data[0] = { r*sin(theta)*cos(phi)+centre[0],
		        r*sin(theta)*sin(phi)+centre[1],
		        r*cos(theta) +centre[2]};
	
	double x(0.), y(0.1);
	while( y> x*x*pow((1.-pow(x,2.)),3.5)){
		x = x_dist(rands);
		y = y_dist(rands);
	}
	V = x*pow((1.+pow(r,2.)),-0.25); // modified here for 2 clusters
	
	
	theta = acos(unit_circle(rands));
	phi = phi_dist(rands);
	data[1] = { V*sin(theta)*cos(phi) + centre_V[0], 
		        V*sin(theta)*sin(phi)+ centre_V[1], 
		        V*cos(theta)+ centre_V[2]};
	
	return data;
}

// Main simulation
int main ( int argc, char *argv[] ){
	omp_set_num_threads( 2 );
	srand(1);
	default_random_engine rands;

	n = 200;
	mass = 1./n;
	dt = 0.1;
	tmax = 100;	
	vector<vector<double> > pos(n,vector<double>(3,0)), vel(n,vector<double>(3,0)), force(n,vector<double>(3,0));
	vector<vector<double> > temp(2,vector<double>(3,0));
	for(int i = 0; i<n; i++){
		temp = plummer_init(rands, {0,0,0},{0,0,0});
		for(int j = 0; j<3;j++){
			pos[i][j] = temp[0][j];
			vel[i][j] = temp[1][j];
		}
	}
	
	return 0;
}
