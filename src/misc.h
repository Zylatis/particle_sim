#pragma once
void test_destructor(int n, int n_runs, Region &sim_region, const vector<current_dtype> &strided_pos, NodePool<OctreeNode> &node_pool, vector<current_dtype> &strided_force){

	for(int x = 0;x<n_runs;x++){
		vector<OctreeNode*> node_map(n);
		vector<OctreeNode*> node_list = {};
		fill(strided_force.begin(), strided_force.end(),0.);


		cout<<"Mem test run: "<<x<<endl;
		// cout<<node_pool.node_pool.size()<<endl;
		OctreeNode* root_node = new (node_pool.get()) OctreeNode(sim_region.xmin, sim_region.xmax, sim_region.ymin, sim_region.ymax, sim_region.zmin, sim_region.zmax);
		cout<< "root node: "<<root_node<<endl;
		for(int i = 0;i<n;i++){
			//cout<<i<<endl;
			root_node->addParticle(i, strided_pos, node_map, node_list, node_pool);		
		}
		// for(auto node: node_map){
			// cout<<setprecision(13)<<node<<"\t"<<(node->centre_of_mass[0])<<endl;			
		// }
		//  for(int i = 0; i<n;++i){
		 	// root_node->calcForce(i, strided_pos, strided_force);	
		 	// cout<<setprecision(13)<<strided_force[i]<<endl;
		//  }
		// cout<<"========="<<endl;
		// cout<<"X"<<endl;
		node_pool.reset();
		//delete root_node;
	}
}

