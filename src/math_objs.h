
// for now assuming all particles are same, maybe gas 'n shit is different later
class NDimVector {
	private:
	
	protected:
		//~ vector<double> elements;
		
	public:
		int len;
		vector<double> elements;
		// possibly bad idea passing as ref, might want to pass resut of return?
		// i forget how L and R values work there...
		NDimVector() {}; 
		NDimVector(int n ) : elements(vector<double>(n,0)) {
			len = n;
		};
		 
		NDimVector( const vector<double> &vec ) :
			elements(vec), len(vec.size()) {};
		
		// vector sum
		NDimVector operator+(const NDimVector &v2) {
			
			NDimVector* sum = new NDimVector(vector<double>(len,0));
			for(int i = 0; i<len; i++){
				sum->elements[i] = elements[i] + v2.elements[i];
			}
			
			return *sum;
		}
		
		// vector add-sum
		void operator+=(const NDimVector &v2) {
			
			NDimVector temp = NDimVector(vector<double>(len,0));
			for(int i = 0; i<len; i++){
				temp.elements[i] = elements[i] + v2.elements[i];
			}
			
			elements = temp.elements;
		}
		
		
		// vector subtract - is there some way to template this?
		NDimVector operator-(const NDimVector &v2) {
			
			NDimVector* sum = new NDimVector(vector<double>(len,0));
			for(int i = 0; i<len; i++){
				sum->elements[i] = elements[i] - v2.elements[i];
			}
			
			return *sum;
		}
		
		// scalar multiply
		NDimVector operator*(double x ) {
		
			NDimVector* prod = new NDimVector(vector<double>(len,0));
			for(int i = 0; i<len; i++){
				prod->elements[i] = elements[i]*x;
			}
			
			return *prod;
		}
		
		// scalar divide
		NDimVector operator/(double x ) {
			NDimVector* sum = new NDimVector(vector<double>(len,0));
			for(int i = 0; i<len; i++){
				sum->elements[i] = elements[i]/x;
			}
			
			return *sum;
		}
		
		// plus equals
		void operator+=(const double &x ) {
			for(int i = 0; i<len; i++){
				elements[i] += x;
			}
		}
		
	
		
		// vector subtract - is there some way to template this?
		// (cool!)
		double& operator[](const int &i) {
			return elements[i];
		}
		
		// dot product (overloading '.' is bad)
		// maybe do cross product too but that needs matrix class for N-dimensional determinant.
		double dot(const NDimVector &v2) {
			double dot_product(0.);
			
			for(int i = 0; i<len; i++){
				dot_product += elements[i]*v2.elements[i];
			}
			
			return dot_product;
		}
		
		// Get magnitude
		double mag(){
			return sqrt( dot(*this));
		}
		
		void resize( int n ){
			elements.resize(n);
			len = n;
		}
		
		void fill( double val ){
			elements = vector<double>(n,0);
		}
	

};
