#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
using namespace std;
#include "headers/catch.hpp"

#include "imports.h"
#include "global_vars.h"
#include "math_objs.h"
#include "particle_class.h"
#include "integrators.h"
#include "io.h"

TEST_CASE( "Final result", "[overall]" ) {
	
	vector<vector<double> > output = FileRead("output/data/100_pos.dat");
	vector<vector<double> > compare = FileRead("final_n200_compare.dat");
	int n = output.size();
	REQUIRE(n == compare.size());
	for(int i = 0; i<n; i++){
		REQUIRE(output[i] == compare[i] );
	}
   
}
	
