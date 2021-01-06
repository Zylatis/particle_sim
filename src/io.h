#pragma once
// #define GET_VARIABLE_NAME(Variable) (#Variable)
#include <string.h>

struct Config {
    int n_particles;
    int n_threads;
    current_dtype tmax;
    current_dtype dt;
    Method method;
};

const Config read_config(const char* filepath){
    // Sloppy config loader. Might be nicer to have some more flexibility (i.e. not exact string matches)
    // but that would prob require STL strings which i'd like to try avoid for now, just to see if i can!

    // Actually reasonable to ask wtf am i doing making this a struct as it's only used once and even then immediately chopped into bits
    // Guess I have to as mixed types? Can't even use a map unless i wanna fuck around with void pointers (vomit)

    Config config_data;
    cout<<"Loading config:"<<endl;
    ifstream in(filepath);

    char str[255];
    char prev_line[255];

    // TODO: refactor into switch/loop
    while(in.getline(str,255))
    {
        if(str[0] == '#'){
            strcpy(prev_line, str);
        } else {
            cout<<prev_line<<endl;
            cout<<str<<endl;
             if(!strcmp(prev_line,"# n_particles")){
                config_data.n_particles =  atoi(str);
             
             } else if(!strcmp(prev_line,"# n_threads")){
                config_data.n_threads =  atoi(str);
   
            } else if(!strcmp(prev_line,"# tmax")){
                config_data.tmax = (current_dtype) atoi(str);
            
            } else if(!strcmp(prev_line,"# method")){
                config_data.method = (Method) atoi(str);
            
            } else if(!strcmp(prev_line,"# dt")){
                config_data.dt = (current_dtype) atof(str);
            }
        }
    }

    return config_data;

}

void write_flat_table(vector< current_dtype> &table, string file_name, int dim, int save_precision = 13){
    if(dim > 3 || dim <=1){
        cout << "Error: invalid dimension given to write_flat_table (" << dim << ")" << endl;
    }
    ofstream stream;
    int size = table.size()/dim;
    stream.open(file_name.c_str(), ios::out);
    for(int i = 0; i < size; i++){
        stream<<setprecision(save_precision);
        for(int k = 0; k<dim; k++){
            stream << table[dim*i+k]<<"\t";
        }
        stream << endl;
    }
    stream.close();

}

void write_state( vector< current_dtype >  &array, string file_name){

    ofstream o;
    string outputFolder = "output/data/";
    int size = array.size()/3;
    string file = outputFolder+file_name+".dat";
    o.open(file.c_str(), ios::out);
    for(int i = 0; i<size; i++){
        o<<setprecision(13)<< array[3*i]<<"\t"<<array[3*i+1]<<"\t"<<array[3*i+2]<<endl;
    }
    o.close();
}

void progress(double perc){
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

    cout << "] " + to_string((int) round(100*perc)) + "\r";
    // cout << "] " + to_string((int) round(100*perc)) + "%, totalE:" + to_string(totalE) +"\r";
}
