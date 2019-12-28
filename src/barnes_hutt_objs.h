

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
		vector<vector<double> > particles;
		int np;
		double total_mass;
		vector<double> com;
		
	public:	
		vector<OctreeNode*> children;
		OctreeNode(vector<vector<double> > inp_particles){
			particles = inp_particles;
			np = particles.size();
		};
		
		int add_particle(vector<double> particle){
			// 
		}
};