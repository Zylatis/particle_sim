using namespace std;

#include "imports.h"
#include "math_objs.h"
#include "io.h"
int FILE_P = 15; // precision of outputs

int n;



// Get delta - r
vector<double> diff_vec(  const vector<vector< double > >&pos_vec , int i, int j){
	
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
vector<vector< double > > force( const vector<vector< double > > &pos_vec ){
	vector<vector< double > >  out(n, vector<double>(3,0));
	
	#pragma omp parallel for
	for(int i = 0; i< n; i++){
		for(int j = 0; j<n; j++){
			if(i!=j){
				vector<double> drvec = diff_vec(pos_vec, j, i);				
				double rmag = get_mag(drvec);
				for(int k = 0; k<3;k++){
					out[i][k] += G*mass*drvec[k]/(eps+pow(rmag,3));
				}
			}
		}
	}
	
	return out;	
}



void do_step( vector<vector< double > > &pos, vector<vector< double > > &vel, double dt){
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			pos[i][k] += vel[i][k]*dt;
		}
	}
	vector<vector< double> > accel = force(pos);
	#pragma omp parallel for
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			vel[i][k] += accel[i][k]*dt;
		}
	}
}

vector<vector<double> > init(  default_random_engine &rands, const vector<double> &centre, const vector<double> &centre_V ){
	
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
	
	//~ V = x*sqrt(2.)*pow((1.+pow(r,2.)),-0.25);
	V = x*pow((1.+pow(r,2.)),-0.25); // modified here for 2 clusters
	
	
	theta = acos(unit_circle(rands));
	phi = phi_dist(rands);
	data[1] = { V*sin(theta)*cos(phi) + centre_V[0], 
		        V*sin(theta)*sin(phi)+ centre_V[1], 
		        V*cos(theta)+ centre_V[2]};
	
	//~ cout<<(V<sqrt(2)*pow((r+1),-.25))<<endl;
	return data;
}

// Main simulation
int main ( int argc, char *argv[] ){
	omp_set_num_threads( 4 );
	srand(1);
	default_random_engine rands;
	n = 200;
	mass = 1./n;
	int step(0), file_n(0);
	double dt(0.1), t(0.), tmax(200.);
	vector<double> blank(3, 0.);
	vector<vector< double > > pos(n,blank), vel(n,blank), accel(n,blank);
	
	for(int i = 0; i<n/2; i++){	
		vector<vector<double> > temp = init( rands,{0,0,0},{0,0,0});
		pos[i] = temp[0];
		vel[i] = temp[1];
	}
	
	for(int i = n/2; i<n; i++){	
		vector<vector<double> > temp = init( rands,{30.,0,0},{-.02,0,0});
		pos[i] = temp[0];
		vel[i] = temp[1];
	}
	
	
	
	accel = force(pos);
	for(int i = 0; i<n; i++){
		for(int k = 0; k<3;k++){
			vel[i][k] += accel[i][k]*dt/2.;
		}
		
	}
	vector<vector<double> > temp;
	double perc = 0;
	write_state(pos, to_string(file_n)+"_pos");
	double wt = get_wall_time();
	while(t<tmax){
		do_step(pos, vel, dt);
		if(step%10==0){
			cout<<t<<endl;
			temp = pos;
			//~ auto out = async(launch::async, write_state, ref( temp ),to_string(file_n)+"_pos");
			write_state(pos, to_string(file_n)+"_pos");
			file_n++;
		}
		t+=dt;
		step++;
	}
	cout<< ( get_wall_time() - wt )<<endl;
	return 0;
}
