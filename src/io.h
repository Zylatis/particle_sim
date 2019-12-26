#include "boost/filesystem.hpp" 
namespace fs = boost::filesystem;

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


void empty_folder( string folder_path, string extension){
    // https://stackoverflow.com/questions/6935279/delete-all-txt-in-a-directory-with-c
    // This needs to be extension-aware so we don't bin our .gitignore files
    fs::path p(folder_path);
    if(fs::exists(p) && fs::is_directory(p))
    {
        fs::directory_iterator end;
        for(fs::directory_iterator it(p); it != end; ++it)
        {
            try
            {
                if(fs::is_regular_file(it->status()) && (it->path().extension().compare(extension) == 0))
                {
                    fs::remove(it->path());

                }
            }
            catch(const std::exception &ex)
            {
                ex;
            }
        }
    }

}
