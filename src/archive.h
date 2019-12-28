
void zero_vec(vector<vector< double > > &v){
	vector<double> blank(3,0);
	for(int i=0;i<v.size();i++){
		fill(v[i].begin(), v[i].end(), 0.);
	}
}


// void force_accumulate_inplace( vector<vector<double> > &a, vector<vector<double> >&b){
// 	int n = a.size();
// 	assert(b.size()==n);
// 	for(int i = 0;i<n;i++){
// 		for(int k = 0; k<3;k++){
// 			a[i][k] += b[i][k];
// 		}		
// 	}
// }

// #pragma omp declare reduction(forceAdd: vector<vector<double> >: force_accumulate_inplace(omp_out, omp_in))
// vector<vector< double > > force3( const vector<vector< double > > &pos_vec, const vector<vector< double > > &vel_vec, const int &n, double &totalE ){
// 	vector<vector< double > >  out(n, vector<double>(3,0));
// 	totalE = 0.;	
// 	#pragma omp parallel for// reduction(forceAdd:out)
// 	for(int i = 0; i< n; i++){
// 		double vmag = get_mag(vel_vec[i]);
// 		for(int j = 0; j<n; j++){
// 			if(i!=j){
// 				vector<double> drvec = diff_vec(pos_vec, j, i);				
// 				double rmag = get_mag(drvec);
				
// 				for(int k = 0; k<3;k++){
// 					out[i][k] += G*mass*drvec[k]/(eps+pow(rmag,3));
// 					// out[j][k] += -G*mass*drvec[k]/(eps+pow(rmag,3));
// 				}
// 			}
// 		}
// 	}
// 	return out;	
// }

// vector<vector< double > > force( const vector<vector< double > > &pos_vec, const vector<vector< double > > &vel_vec, const int &n, double &totalE, vector<vector<vector<double > > > &force_grid ){
// 	vector<vector< double > >  out(n, vector<double>(3,0));
// 	totalE = 0.;	
// 	#pragma omp parallel for
// 	for(int i = 0; i< n; i++){
// 		double vmag = get_mag(vel_vec[i]);
// 		for(int j = 0; j<n; j++){
// 			if(i!=j){
// 				vector<double> drvec = diff_vec(pos_vec, j, i);				
// 				double rmag = get_mag(drvec);
				
// 				for(int k = 0; k<3;k++){
// 					out[i][k] += G*mass*drvec[k]/(eps+pow(rmag,3));
// 					// out[j][k] += -G*mass*drvec[k]/(eps+pow(rmag,3));
// 				}
// 			}
// 		}
// 	}

// 	return out;	
// }
