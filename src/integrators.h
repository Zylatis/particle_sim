// leapfrog
// does half step for x'' first and then proceeds

void leap_frog( vector<Particle*> &swarm ){
	for( auto particle : swarm ){
		for(int i = 0; i<3; i++){
			(particle->pos_)[i] += (particle->vel_)[i]*dt;
		}
	}	
	
	
	for( auto particle : swarm ){
		particle->force( swarm ); 
	}
	
	
	for( auto particle : swarm ){
		for(int i = 0; i<3; i++){
			(particle->vel_)[i] += (particle->force_)[i]*dt;
			//~ particle->vel_ += (particle->force_)*dt;
		}			
	}
	
	
	t+=dt;
	step++;
	
	
}

void leap_frog_init( vector<Particle*> &swarm  ){
	
	for( auto particle : swarm ){
		particle->force( swarm ); 
	}
	
	for( auto particle : swarm ){
		particle->vel_ += particle->force_*dt*0.5;
	}
}
