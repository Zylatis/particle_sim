

// class Octree {
// 	private:

// 		vector<vector<double> > particles;
// 		class OctreeNode {
// 			private:
// 				double xmin, xmax, ymin, ymax, zmin, zmax;
// 				vector<vector<double> > particles;
// 				double total_mass;
// 				vector<double> com;

// 			public:	
// 				vector<OctreeNode*> children;
// 				OctreeNode(vector<vector<double> > inp_particles){
// 					children.resize(8);
// 					particles = inp_particles;
// 				};
				
// 		};
// 	protected:
// 	public:	
// 		Octree(){};
// 		Octree(vector<vector<double> > &pos){
// 			particles = pos;
// 		}

// 	void build(){
// 		OctreeNode* root = new OctreeNode(particles);
// 	}
// };


template <typename T> 
void print_container(T &v){
	for(int i = 0; i<v.size();i++){
		cout<<v[i]<<"\t";
	}
	cout<<endl;
}


class OctreeNode {
	private:
		// Boundary of octant
		double xmin, xmax, ymin, ymax, zmin, zmax;
		
	public:	
		// CoM for particles in octant
		array<double,3> centre_of_mass;

		// Ids (from 0 to N) of particles in octant
		vector<int> particle_ids;

		// List of child nodes
		vector<OctreeNode*> children;

		// Booking keeping of parent node
		OctreeNode* parent;

		// Constructor
		OctreeNode(double i_xmin, double i_xmax, double i_ymin, double i_ymax, double i_zmin, double i_zmax){
			xmin = i_xmin;
			xmax = i_xmax;

			ymin = i_ymin;
			ymax = i_ymax;

			zmin = i_zmin;
			zmax = i_zmax;
		};

		// Subdivide a given 3D space into octants
		vector<vector<double>> calcOctants(){
			double dx = (xmax-xmin)*0.5;
			double dy = (ymax-ymin)*0.5;
			double dz = (zmax-zmin)*0.5;
			
			vector<vector<double> > children_boxes = {
					{xmin,xmin + dx}, {ymin, ymin + dy}, {zmin, zmin+dz},
					{xmin + dx,xmax}, {ymin, ymin + dy}, {zmin, zmin+dz},
					{xmin,xmin + dx}, {ymin + dy, ymax}, {zmin, zmin+dz},
					{xmin + dx,xmax}, {ymin + dy, ymax}, {zmin, zmin+dz},

					{xmin,xmin + dx}, {ymin, ymin + dy}, {zmin+dz, zmax},
					{xmin + dx,xmax}, {ymin, ymin + dy}, {zmin+dz, zmax},
					{xmin,xmin + dx}, {ymin + dy, ymax}, {zmin+dz, zmax},
					{xmin + dx,xmax}, {ymin + dy, ymax}, {zmin+dz, zmax}
				};

			return children_boxes;
		}

		// Check if particle falls with in given octant
		bool particleInNode(double x, double y, double z){
			return (x<= xmax && x> xmin && y <= ymax && y > ymin && z <= zmax && z> zmin);
		}

		// Function to be called recursively to add a particle to the tree (called on root node externally)
		int add_particle(int particle, const vector<double> &strided_pos, vector<OctreeNode*> node_map){

			// Get 3D coords for incoming particle
			double x(strided_pos[3*particle]), y(strided_pos[3*particle+1]), z(strided_pos[3*particle+2]);

			// Add this particle to the list for this octant
			int nparticles = particle_ids.size();

			// We add this particle to the octant *after* getting the count just so the if statements below make sense
			particle_ids.push_back(particle);
			
			// If currently chosen octant is empty, this becomes a leaf node (for now) for this particle
			if(nparticles == 0){
				// empty region, this particle occupies it and it becomes a leaf node

				// Put this octant node pointer into the map from particle_id -> node so we can trace back wtf is going on
				node_map[particle] = this;

				// Set CoM
				centre_of_mass = {x, y, z};

				// Return up a level
				return 0;

			} else if(nparticles == 1){
				// Region currently a leaf node but no more! we add a particle to it and subdivide the region into empty octants, adding particles into relevant ones
				
				// Get octant info and loop over regions creating a new node for each one and seeing which of the (now two) particles we have goes in which one
				auto octants = calcOctants();
				for(int i = 0 ; i<8; i++){
					OctreeNode *temp = new OctreeNode( octants[3*i + 0][0], octants[3*i + 0][1], octants[3*i + 1][0], octants[3*i + 1][1], octants[3*i + 2][0], octants[3*i + 2][1] );

					// Set parent of those new nodes to this current node for book keeping
					temp->parent = this;

					// Add new nodes to children list
					children.push_back(temp);
					// As we go along making the quadrants, check which one it should be put into, and recursively add it to that quadrant
					auto prev_particle = particle_ids[0];
					// cout<<prev_particle<<endl;
					// cout<<strided_pos[3*prev_particle]<<"\t"<<strided_pos[3*prev_particle+1]<<"\t"<<strided_pos[3*prev_particle+2]<<endl;
					// Add the particle that was originally in the parent octant (before adding the new particle) to correct octant
					if(children[i]->particleInNode(strided_pos[3*prev_particle],strided_pos[3*prev_particle+1],strided_pos[3*prev_particle+2])){
						// cout<<"prev"<<"\t"<<prev_particle<<endl;
						children[i]->add_particle(prev_particle, strided_pos, node_map);
					}
					// Now add new particle to relevant octant
					if(children[i]->particleInNode(x,y,z)){
						children[i]->add_particle(particle, strided_pos, node_map);
					}
				}

				// assuming mass = 1 for now, update COM with new particle
				for(int k = 0; k<3; k++){
					centre_of_mass[k] += strided_pos[3*particle+k];
					centre_of_mass[k] /= 2.;
				}

				return 0;

			} else if(nparticles>1){
				// This is an internal node so we need to find which (assumed existing) octant this particle should go into
				// Could this be refactored into the above loop doing the same check?
				for(int i = 0; i<8;i++){
					if(children[i]->particleInNode(x,y,z)){
						children[i]->add_particle(particle, strided_pos, node_map);
					}
				}

				return 0;
			}
		}
};