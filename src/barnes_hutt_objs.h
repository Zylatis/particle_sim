

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



class OctreeNode {
	private:
		double xmin, xmax, ymin, ymax, zmin, zmax;
		vector<int> particle_ids;
		array<double,3> centre_of_mass;
	public:	
		vector<OctreeNode*> children;
		OctreeNode(double i_xmin, double i_xmax, double i_ymin, double i_ymax, double i_zmin, double i_zmax){
			xmin = i_xmin;
			xmax = i_xmax;

			ymin = i_ymin;
			ymax = i_ymax;

			zmin = i_zmin;
			zmax = i_zmax;

		};

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

		bool particleInNode(double x, double y, double z){
			return (x<= xmax && x>= xmin && y <= ymax && y >= ymin && z <= zmax && z>= zmin);
		}

		int add_particle(int particle, const vector<double> &strided_pos){
			int nparticles = particle_ids.size();
			double x(strided_pos[3*particle]), y(strided_pos[3*particle+1]), z(strided_pos[3*particle+2]);
			if(nparticles == 0){
				// empty region, this particle occupies it and it becomes a leaf node
				particle_ids.push_back(particle);
				centre_of_mass = {x, y, z};
				return 0;

			} else if(nparticles == 1){
				// region currently a leaf node but no more! we add a particle to it and subdivide the region into empty octants, adding particles into relevant ones
				auto octants = calcOctants();
				for(int i = 0 ; i<8; i++){
					OctreeNode *temp = new OctreeNode(xmin, xmax, ymin, ymax, zmin, zmax);
					children.push_back(temp);
				}
				// assuming mass = 1 for now
				centre_of_mass[0] += x;
				centre_of_mass[1] += y;
				centre_of_mass[2] += z;
			}
		}
};