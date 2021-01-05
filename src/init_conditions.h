#ifndef init_conditions
#define init_conditions

current_dtype region_size = 3000;
struct Region {
	    current_dtype xmin = -region_size;
	    current_dtype ymin = -region_size;
	    current_dtype zmin = -region_size;

	    current_dtype xmax = region_size;
	    current_dtype ymax = region_size;
	    current_dtype zmax = region_size;
	};

bool particleInRegion(double x, double y, double z, const Region &region){ // Should really call this from Barnes Hutt routine too 
	return (x<= region.xmax && x> region.xmin && y <= region.ymax && y > region.ymin && z <= region.zmax && z> region.zmin);
}


vector<vector<current_dtype> > init(  default_random_engine &rands, const vector<current_dtype> &centre, const vector<current_dtype> &centre_V, const Region &sim_region ){
	
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

#endif