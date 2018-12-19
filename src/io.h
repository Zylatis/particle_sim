
void write_state( vector< vector< double>  >  &array, string file_name){

	ofstream o;
	string outputFolder = "output/data/";
	int size = array.size();
	string file = outputFolder+file_name+".dat";
	o.open(file.c_str(), ios::out);
	for(int i = 0; i<size; i++){
		o<< array[i][0]<<"\t"<<array[i][1]<<"\t"<<array[i][2]<<endl;
	}
	o.close();
}

void write_state(const  vector<Particle*> &swarm, const string &file_name  ){
	vector<double> pos(n,0), vec(n,0);
	ofstream o;
	string outputFolder = "output/data/";
	int size = swarm.size();
	string file = outputFolder+file_name+".dat";
	o.open(file.c_str(), ios::out);
	for( auto &particle : swarm ){
		o<< particle->pos_[0]<<"\t"<<particle->pos_[1]<<"\t"<<particle->pos_[2]<<endl;
	}
	o.close();
}

/////////////////////////////////////////////////////////////////////////////////////
//Read in BO wfn file
vector< vector<double> > FileRead( string filename){
    vector< vector<double> > vvv;
    int i = 0;
    string line;

    fstream in(filename.c_str());
    if(!in.is_open()){
		cout<<"Bugger: unable to open file "+filename+" - exiting."<<endl;
		exit(0);
	}
	while (std::getline(in, line))
    {
        double value;
        stringstream ss(line);

        vvv.push_back(vector<double>());

        while (ss >> value)
        {
            vvv[i].push_back(value);
        }
        ++i;
    }
    return vvv;
}
