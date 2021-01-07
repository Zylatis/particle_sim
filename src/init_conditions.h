#pragma once

// struct Region {
// 	    current_dtype xmin, xmax, ymin, ymax, zmin, zmax; // Doesn't really gel well with variable dim but meh
// 	};

struct Universe {
	vector<current_dtype> strided_pos;
	vector<current_dtype> strided_force;
	vector<current_dtype> strided_vel;
	current_dtype mass = 0.001; // TODO: promote to vector of particle masses generated from some distribution
	int n_particles;
};

void init_universe(Universe &universe, int n_particles, int dim){
	// Tempted to fold into class but classes with just one function are weird...
	// However: we might want to generate the masses from a distribution. Prob should be in init...
	universe.n_particles = n_particles;
	auto n = dim*n_particles;
	universe.strided_pos.resize(n);
	universe.strided_vel.resize(n);
	universe.strided_force.resize(n);
}

bool particleInRegion(double x, double y, double z, const array<current_dtype, 6> &region){ // Should really call this from Barnes Hutt routine too 
	return (x<= region[1] && x> region[0] && y <= region[3] && y > region[2] && z <= region[5] && z> region[4]);
}

vector<vector<current_dtype> > init(  default_random_engine &rands, const vector<current_dtype> &centre, const vector<current_dtype> &centre_V,  array<current_dtype, 6> &sim_region ){
	
	vector<vector< current_dtype> > data = {vector<current_dtype>(3,0), vector<current_dtype>(3,0)};

	uniform_real_distribution<current_dtype> r_dist(0.,1.), unit_circle(-1,1), phi_dist(0,2.*3.14159), x_dist(0.,1.), y_dist(0.,0.1);
	current_dtype theta, r, phi, V;

	theta = acos(unit_circle(rands));
	phi = phi_dist(rands);
	r = 1./(sqrt(pow(r_dist(rands),-2./3.)-1.));
	data[0] = { r*sin(theta)*cos(phi)+centre[0],
		        r*sin(theta)*sin(phi)+centre[1],
		        r*cos(theta) +centre[2]};
	
	current_dtype x(0.), y(0.1);
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
	
	// If we find a point out of bounds, recursively call the function again until it's inside the required region
	if(!particleInRegion(data[0][0],data[0][1],data[0][2], sim_region)){
		cout<<"Found out of bounds point, resampling:"<<endl;
		data = init( rands, centre, centre_V, sim_region );
	}	        

	return data;
}
