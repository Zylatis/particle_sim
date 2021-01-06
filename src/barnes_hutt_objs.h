#pragma once

template <typename T> 
class NodePool {
	// Very hacky memory pool for octree nodes
	private:
		unsigned int loc = 0;
		vector<T> node_pool; // better way to have a buffer? I don't understand the mem footprint of this, better to do as plain array?

	public:

		NodePool(unsigned long int n){
			node_pool.reserve(n);
		};

		~NodePool(){
			node_pool.clear();
		}

		T* get(){
			loc++;
			return &node_pool[loc-1];
		}

		void reset(){
			for(int i = 0; i<loc;++i){
				node_pool[i].reset();
			}
			loc = 0;
		}

		double pool_mem_size(){
			return (sizeof(node_pool) + (sizeof(T) * node_pool.capacity()))/ 1024 / 1024;

		}
};


// TODO: ensure all dtypes aligned with current_dtype (can't get some highlighting for that shit?)
template <typename T> 
void print_container(T &v){
	for(int i = 0; i<v.size();i++){
		cout<<v[i]<<"\t";
	}

	cout<<endl;
}

// TODO: template dimension
class OctreeNode {
	private:
		// Boundary of octant
		// double xmin, xmax, ymin, ymax, zmin, zmax, s; // Should really store in an array
		double s;
		array<current_dtype> boundaries(8,0.); 
		double theta = 0.0; // TODO: move to config or solver type init, or to calc_force 
	public:	
		// CoM for particles in octant
		array<double,3> centre_of_mass;

		// Ids (from 0 to N) of particles in octant
		bool is_leaf = false;
		int n_particles = 0;
		int leaf_particle_id;

		// List of child nodes
		array<OctreeNode*, 8> children = {};

		// Booking keeping of parent node
		OctreeNode* parent;

		void reset(){
			children = {};
			centre_of_mass = {};
			fill(boundaries.begin(), boundaries.end(),0.);

			s = 0;
		}

		// Constructor
		// Given that the region is always square we could refactor this to use 's' instead
		OctreeNode(const array<current_dtype> &boundaries){
			this->boundaries = boundaries;
			s = boundaries[1] - boundaries[1];
		};

		~OctreeNode(){
			for(auto node : children){
				if(node){ //  With the empty array we might have nulls which will fail on delete here so need to check
					node->parent = NULL;
					delete node;
				}
			}
			// children = {}; // seems to add memory overhead and probably isn't needed?

		}

		// void printBoundaries(){
		// 	cout<<xmin<<"\t"<<xmax<<endl;
		// 	cout<<ymin<<"\t"<<ymax<<endl;
		// 	cout<<zmin<<"\t"<<zmax<<endl;
		// 	cout<<endl;
		// }

		// Subdivide a given 3D space into octants
		vector<vector<double>> calcOctants(){
			double dx = s/2.;
			double dy = s/2.;
			double dz = s/2.;
			
			vector<vector<double> > children_boxes = {
				// TODO make flat list
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
			array<double,3> drvec = {0.,0.,0.};
			int x0 = 3*particle;
			double rmag;
            // if(is_leaf){
             //   cout<<"leaf particle " << leaf_particle_id << " incoming particle " << particle << endl;
            // }     
			// If current node is a leaf node, and particle in node is not the target particle, calculate two body force and add
			if(is_leaf && (leaf_particle_id != particle)){
				double x(strided_pos[x0]), y(strided_pos[x0+1]), z(strided_pos[x0+2]);

				// Get the particle ID of particle in this leaf node
				int p2 = leaf_particle_id;
				
				// Calc dr
				for(int k = 0; k<3;k++){
					drvec[k] = strided_pos[3*p2+k] - strided_pos[x0+k];				
				}

				// Calc |dr|
				rmag = sqrt(drvec[0]*drvec[0] + drvec[1]*drvec[1] + drvec[2]*drvec[2]);

				// Add force components
				for(int k = 0; k<3;k++){
					double val = G*mass*drvec[k]/(eps+rmag*rmag*rmag);

					strided_force[x0+k] += val;
				}

                //cout<<"New BH target particke"<< particle << " leaf node particle " << leaf_particle_id << " rmag " << rmag<<endl; 

			} else if(n_particles>1){

				// else, calc s/d
				for(int k = 0; k<3;k++){
					drvec[k] = centre_of_mass[k] - strided_pos[x0+k];
				}

				rmag = sqrt(drvec[0]*drvec[0] + drvec[1]*drvec[1] + drvec[2]*drvec[2]);

				double sd_frac = s/rmag;

				// if octant width/dist between target particle and node CoM < threshold 
				if(sd_frac < theta){
					// Consider this node as a single body, compute force as force from particle of mass total mass at CoM
					for(int k = 0; k<3;k++){
						double val = G*mass*n_particles*drvec[k]/(eps+rmag*rmag*rmag);
						strided_force[x0+k] += val;
					}

				} else {
					// call this procedure on each of this nodes children 
					// Doesn't seem cache friendly to basically DFS here...
					for(auto node : children){
						node->calcForce(particle, strided_pos, strided_force);
					}
				}
			} 
		}

		// Function to be called recursively to add a particle to the tree (called on root node externally)
		int addParticle(int particle, const vector<double> &strided_pos, vector<OctreeNode*> &node_map, vector<OctreeNode*> &node_list, NodePool<OctreeNode> &node_pool){
			
			// Get 3D coords for incoming particle
			int x0 = 3*particle;
			double x(strided_pos[x0]), y(strided_pos[x0+1]), z(strided_pos[x0+2]);
			n_particles +=1;
			
			// If currently chosen octant is empty (before we added particle), this becomes a leaf node (for now) for this particle
			if(n_particles == 1){
				// empty region, this particle occupies it and it becomes a leaf node
				leaf_particle_id = particle;
                is_leaf = true;

				// Put this octant node pointer into the map from particle_id -> node so we can trace back wtf is going on
				node_map[particle] = this;

				// Set CoM
				centre_of_mass = {x, y, z};

				// Return up a level
				return 0;

			} else if(n_particles == 2){
				// Region currently a leaf node but no more! we add a particle to it and subdivide the region into empty octants, adding particles into relevant ones
                is_leaf = false;
				
				// Get octant info and loop over regions creating a new node for each one and seeing which of the (now two) particles we have goes in which one
				auto octants = calcOctants();
				for(int i = 0 ; i<8; i++){
					
					// Create new node (shock, raw pointers! Look at my face, this is my caring face. See how much I care?)
					// TODO: use a custom memory allocator for this to avoid shitloads of allocs/deallocs
					auto pool_mem = node_pool.get();
					int i0 = 3*i;
					OctreeNode *temp = new (pool_mem) OctreeNode( octants[i0 + 0][0], octants[i0 + 0][1], octants[i0 + 1][0], octants[i0 + 1][1], octants[i0 + 2][0], octants[i0 + 2][1] );

					// Keep track of all our nodes mostly for book-keeping/debugging
					node_list.push_back(temp);
					// Set parent of those new nodes to this current node for book keeping
					// temp->parent = this;

					// Add new nodes to children list
					children[i] = temp;

					// 2/1/2021 change to remove particle_ids hashmap
					// We only ever hit this part if we have 2 particles so we just need to add each. 
					// We know the coords of the new one (passed to this functon) and, until the bit below updates it, the old particle coords are just the CoM of this node
					if(children[i]->particleInNode(centre_of_mass[0],centre_of_mass[1],centre_of_mass[2])){
						children[i]->addParticle(leaf_particle_id, strided_pos, node_map, node_list, node_pool);
					}

					if(children[i]->particleInNode(strided_pos[x0], strided_pos[x0+1], strided_pos[x0+2])){
						children[i]->addParticle(particle, strided_pos, node_map, node_list, node_pool);
					}
				}

				// assuming mass = 1 for now, update COM with new particle
				for(int k = 0; k<3; k++){
					centre_of_mass[k] += strided_pos[x0+k];
					centre_of_mass[k] /= 2.;
				}
                leaf_particle_id = NULL;

				return 0;

			} else if(n_particles>2){
                is_leaf = false;
                leaf_particle_id = NULL;
				// This is an internal node so we need to find which (assumed existing) octant this particle should go into
				// Could this be refactored into the above loop doing the same check?
				for(int i = 0; i<8;i++){
					if(children[i]->particleInNode(x,y,z)){
						children[i]->addParticle(particle, strided_pos, node_map, node_list, node_pool);
						break; //  don't need to check any other nodes for this new particle
					}

				}

				return 0;
			}

			return 1;
		}
};
