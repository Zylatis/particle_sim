#include <unordered_set>

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
		double xmin, xmax, ymin, ymax, zmin, zmax, s;
		double theta = 0.;
	public:	
		// CoM for particles in octant
		array<double,3> centre_of_mass;

		// Ids (from 0 to N) of particles in octant
		unordered_set<int> particle_ids;

		// List of child nodes
		vector<OctreeNode*> children;

		// Booking keeping of parent node
		OctreeNode* parent;

		// Constructor
		// Given that the region is always square we could refactor this to use 's' instead
		OctreeNode(double i_xmin, double i_xmax, double i_ymin, double i_ymax, double i_zmin, double i_zmax){
			xmin = i_xmin;
			xmax = i_xmax;

			ymin = i_ymin;
			ymax = i_ymax;

			zmin = i_zmin;
			zmax = i_zmax;

			s = xmax - xmin;
		};

		void printBoundaries(){
			cout<<this<<"\t"<<particle_ids.size()<<endl;
			cout<<xmin<<"\t"<<xmax<<endl;
			cout<<ymin<<"\t"<<ymax<<endl;
			cout<<zmin<<"\t"<<zmax<<endl;
			cout<<endl;
		}
		// Subdivide a given 3D space into octants
		vector<vector<double>> calcOctants(){
			double dx = s/2.;
			double dy = s/2.;
			double dz = s/2.;
			// double dx = (xmax-xmin)*0.5;
			// double dy = (ymax-ymin)*0.5;
			// double dz = (zmax-zmin)*0.5;
			
			vector<vector<double> > children_boxes = {
					{xmin,xmin + dx}, {ymin, ymin + dy}, {zmin, zmin + dz},
					{xmin + dx,xmax}, {ymin, ymin + dy}, {zmin, zmin + dz},
					{xmin,xmin + dx}, {ymin + dy, ymax}, {zmin, zmin + dz},
					{xmin + dx,xmax}, {ymin + dy, ymax}, {zmin, zmin + dz},

					{xmin,xmin + dx}, {ymin, ymin + dy}, {zmin + dz, zmax},
					{xmin + dx,xmax}, {ymin, ymin + dy}, {zmin + dz, zmax},
					{xmin,xmin + dx}, {ymin + dy, ymax}, {zmin + dz, zmax},
					{xmin + dx,xmax}, {ymin + dy, ymax}, {zmin + dz, zmax}
				};

			return children_boxes;
		}

		// Check if particle falls with in given octant
		bool particleInNode(double x, double y, double z){
			return (x<= xmax && x> xmin && y <= ymax && y > ymin && z <= zmax && z> zmin);
		}

		void calcForce(int particle, const vector<double> &strided_pos, vector<double> &strided_force){

			array<double,3> drvec ={0.,0.,0.};
			double rmag(-1);
			double x(strided_pos[3*particle]), y(strided_pos[3*particle+1]), z(strided_pos[3*particle+2]);
			
			// If current node is a leaf node, and particle in node is not the target particle, calculate two body force and add
			if(particle_ids.size() == 1 && particle_ids.find(particle) == particle_ids.end()){
				// Get the particle ID of particle in this leaf node
				int p2 = *particle_ids.begin();
				// Calc dr
				for(int k = 0; k<3;k++){
					drvec[k] = strided_pos[3*p2+k] - strided_pos[3*particle+k];				
				}

				// Calc |dr|
				rmag = sqrt(drvec[0]*drvec[0] + drvec[1]*drvec[1] + drvec[2]*drvec[2]);

				// Add force components
				for(int k = 0; k<3;k++){
					double val = G*mass*drvec[k]/(eps+rmag*rmag*rmag);

					strided_force[3*particle+k] += val;
					// if(particle==0){
					// 	cout<<val<<"\t";
					// 	if(k==2){
					// 		cout<<"\t"<<strided_force[3*particle+k]<<"\t"<<strided_force[3*particle+k]<<"\t"<<p2<<endl;
					// 		if(p2==193){
					// 			cout<<"XXXXXXXXXXXXXxX"<<endl;
					// 		}
					// 	}
					// }

				}

			} else if(particle_ids.size()>1){
				// else, calc s/d
				for(int k = 0; k<3;k++){
					drvec[k] = centre_of_mass[k] - strided_pos[3*particle+k];
					
				}
				rmag = sqrt(drvec[0]*drvec[0] + drvec[1]*drvec[1] + drvec[2]*drvec[2]);

				double sd_frac = s/rmag;

				// if octant width/dist between target particle and node CoM < threshold 
				if(sd_frac < theta){
					// cout<<"considering node as single body"<<endl;
					// Consider this node as a single body, compute force as force from particle of mass total mass at CoM
					for(int k = 0; k<3;k++){
						double val = G*mass*particle_ids.size()*drvec[k]/(eps+rmag*rmag*rmag);
						strided_force[3*particle+k] += val;


					}
				} else {
					// call this procedure on each of this nodes children
					for(auto node : children){
						node->calcForce(particle, strided_pos, strided_force);
					}
				}
			// } else if(particle_ids.find(particle) != particle_ids.end() || (particle_ids.size()==0)){


			} 
			// else {
				// cout<<"========"<<endl;
				// cout<<particle<<endl;
				// cout<<particle_ids.size()<<endl;
				// for(auto x : particle_ids){
				// 	cout<<x<<endl;
				// }
				// cout<<"========"<<endl;
				// exit(0);
			// }

		}

		// Function to be called recursively to add a particle to the tree (called on root node externally)
		int addParticle(int particle, const vector<double> &strided_pos, vector<OctreeNode*> &node_map, vector<OctreeNode*> &node_list){

			// Get 3D coords for incoming particle
			double x(strided_pos[3*particle]), y(strided_pos[3*particle+1]), z(strided_pos[3*particle+2]);

			// Add this particle to the list for this octant
			int nparticles = particle_ids.size();

			// We add this particle to the octant *after* getting the count just so the if statements below make sense
			particle_ids.insert(particle);
			
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
					
					// Create new node (shock, raw pointers! Look at my face, this is my caring face. See how much I care?)
					OctreeNode *temp = new OctreeNode( octants[3*i + 0][0], octants[3*i + 0][1], octants[3*i + 1][0], octants[3*i + 1][1], octants[3*i + 2][0], octants[3*i + 2][1] );

					// Keep track of all our nodes mostly for book-keeping/debugging
					node_list.push_back(temp);
					// Set parent of those new nodes to this current node for book keeping
					temp->parent = this;

					// Add new nodes to children list
					children.push_back(temp);

					// As we go along making the octants, check which one it should be put into, and recursively add it to that octanct
					for(auto p_id : particle_ids){
						// This was done this way because we moved to hash set to store particles so no notion of getting the 'first' particle
						// However, there is a perf hit here because we are accessing strided_pos when already have x,y,z for that particle
						if(children[i]->particleInNode(strided_pos[3*p_id],strided_pos[3*p_id+1],strided_pos[3*p_id+2])){
							children[i]->addParticle(p_id, strided_pos, node_map, node_list);
						}
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
						children[i]->addParticle(particle, strided_pos, node_map, node_list);
					}
				}

				return 0;
			}

			return 1;
		}
};