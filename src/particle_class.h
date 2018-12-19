// for now assuming all particles are same, maybe gas 'n shit is different later
class Particle {
	private:
	protected:
	public:
		double mass_;
		NDimVector pos_, vel_, force_;
		int dim;
	
	// pass in as N-dim STL vector
	Particle( double mass, vector<double> pos, vector<double> vel ) :
		mass_( mass ), pos_( NDimVector( pos ) ), vel_( NDimVector( vel ) ), dim( pos_.len) {
			force_.resize( dim );
		};	 // correct way to do this?
	
	// calculate force vector on given particle
	void force( vector<Particle*> &swarm ){		
		NDimVector dr_vec(3), temp( dim );
		double mag;
		//~ force_.fill(0.);
		for(auto particle : swarm ){
			for(int i = 0; i<dim; i++){
				dr_vec[i]= (particle->pos_)[i] - pos_[i];
			}
			mag = dr_vec.mag();	
			for( int i = 0; i < dim; i++ ){
			if(mag!=0){
				temp[i] += dr_vec[i]*G*mass/(eps+pow(mag,3));
				}
			}				
		}
		force_ = temp;
	}
};
