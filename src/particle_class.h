// for now assuming all particles are same, maybe gas 'n shit is different later
class Particle {
	private:
	protected:
	public:
		double mass_;
		NDimVector pos_, vel_;
		int dim;
	
	// pass in as N-dim STL vector
	Particle( double mass, vector<double> pos, vector<double> vel ) :
		mass_( mass ), pos_( NDimVector( pos ) ), vel_( NDimVector( vel ) ), dim( pos_.len) {};	 // correct way to do this?
	
	// calculate force vector on given particle
	void force( vector<Particle*> &particles ){		
		NDimVector dr_vec, F;
		double mag;
		for(auto particle : particles ){
			dr_vec = particle->pos_ - pos_;
			mag = dr_vec.mag();
			
			for( int i = 0; i < dim; i++ ){
				F[i] = dr_vec[i]/(eps+pow(mag,3));
			}
		}
	}

};
