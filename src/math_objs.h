
// for now assuming all particles are same, maybe gas 'n shit is different later
// basically this is valarray...hurrr
// Needs expression templates!
class NDimVector {
	private:
	
	protected:
		vector<current_dtype> elements;
		
	public:
		int len;

		// possibly bad idea passing as ref, might want to pass resut of return?
		// i forget how L and R values work there...
		NDimVector() {}; 
		NDimVector( const vector<current_dtype> &vec ) :
			elements(vec), len(vec.size()) {};
		
		// vector sum
		NDimVector operator+(const NDimVector &v2) {
			
			NDimVector* sum = new NDimVector(vector<current_dtype>(len,0));
			for(int i = 0; i<len; i++){
				sum->elements[i] = elements[i] + v2.elements[i];
			}
			
			return *sum;
		}
		
		// vector subtract - is there some way to template this?
		NDimVector operator-(const NDimVector &v2) {
			
			NDimVector* sum = new NDimVector(vector<current_dtype>(len,0));
			for(int i = 0; i<len; i++){
				sum->elements[i] = elements[i] - v2.elements[i];
			}
			
			return *sum;
		}
		
		// vector subtract - is there some way to template this?
		// (cool!)
		current_dtype& operator[](int i) {
			return elements[i];
		}
		
		// dot product (overloading '.' is bad)
		// maybe do cross product too but that needs matrix class for N-dimensional determinant.
		current_dtype dot(const NDimVector &v2) {
			current_dtype dot_product(0.);
			
			for(int i = 0; i<len; i++){
				dot_product += elements[i]*v2.elements[i];
			}
			
			return dot_product;
		}
		
		// Get magnitude
		current_dtype mag(){
			return sqrt( dot(*this));
		}
	

};
