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

void write_state( vector< double >  &array, string file_name){

    ofstream o;
    string outputFolder = "output/data/";
    int size = array.size()/3;
    string file = outputFolder+file_name+".dat";
    o.open(file.c_str(), ios::out);
    for(int i = 0; i<size; i++){
        o<< array[3*i]<<"\t"<<array[3*i+1]<<"\t"<<array[3*i+2]<<endl;
    }
    o.close();
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


void progress(double perc, double totalE){
    //https://stackoverflow.com/questions/44987147/progress-bar-c
    int bar_l = 30;
    int pos = perc * bar_l;

    cout << "[";
    for(int i=0; i != bar_l; ++i)
    {
        if(i < pos)
            cout << "#";
        else
            cout << " ";
    }
    cout << "] " + to_string((int) round(100*perc)) + "%, totalE:" + to_string(totalE) +"\r";
}



