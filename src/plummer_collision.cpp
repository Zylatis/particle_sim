#define current_dtype double // should really be defined in config but whatever 
#define FILE_P 15; // precision of outputs

int thread_id, n_threads; // ugly globals

using namespace std; // heresy
#pragma omp threadprivate( thread_id )

#include "imports.h"
#include <sstream>
#include <unordered_map>
#include "io.h"
#include "math_objs.h"
#include "init_conditions.h"
#include "integrator.h"

#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_glfw.h"
#include "include/imgui/imgui_impl_opengl3.h"
#include "include/glad.h"
#include <GLFW/glfw3.h>


static bool InStr(const std::string &source, const std::string &target){
  if(source.find(target) != std::string::npos){
    return true;
  } else {
    return false;
  }
}

struct ShaderProgramSource
{
  std::string VertexSource;
  std::string FragmentSource;
};

static ShaderProgramSource ImportShader(const std::string filepath){
  std::ifstream file(filepath);
  std::string line;
  std::stringstream ss[2];

  std::unordered_map<std::string, int> ShaderType = {
    {"NONE",-1},
    {"VERTEX",0},
    {"FRAGMENT",1}
  };
  std::string type = "NONE";
  while(getline(file, line)){
    if(InStr(line, "#shader")){
      if(InStr(line, "vertex")){
        type = "VERTEX";
      } else if (InStr(line, "fragment")){
        type = "FRAGMENT";
      } else {
        std::cout<<"Invalid shader type found"<<std::endl;
        exit(0);
      }
    } else{
      ss[ShaderType[type]] << line <<'\n';
    }
  }
  return { ss[ShaderType["VERTEX"]].str(),  ss[ShaderType["FRAGMENT"]].str()};
}


static unsigned int CompileShader(unsigned int type, const std::string& source){
 
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str(); // pointer to start of string
 
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if(result == GL_FALSE){
    int l;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &l);
    char* message = (char*)alloca(l*sizeof(char));
    glGetShaderInfoLog(id, l, &l, message);
    std::cout<< "Failed to compile  " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
    std::cout<<message<<std::endl;
    glDeleteShader(id);
    return 0;
  }

  return id;
}


static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

// Main simulation
int main ( int argc, char *argv[] ){


	auto config = read_config("test.config");
	n_threads = config.n_threads;
	omp_set_num_threads( n_threads );

	// initialise threads with ids
	#pragma omp parallel
	{
		thread_id = omp_get_thread_num();
	}

	srand(1);
	default_random_engine rands;
	int n = config.n_particles; 
	mass = 1./n;
	int step(0), file_n(0);
	current_dtype dt(0.1f), t(0.), totalE(0.), tmax(config.tmax);//tmax( vm["tmax"].as<int>() );

	vector<current_dtype> strided_pos(3*n,0.), strided_vel(3*n,0.), strided_force(3*n,0.);
	vector<vector<current_dtype> > strided_force_threadcpy(n_threads, vector<current_dtype>(3*n,0.));
	// vector<current_dtype>  strided_dt_threadcpy(n_threads*3);
	// vector<OctreeNode*> node_map(n);
	// current_dtype xmin(-16), xmax(16), ymin(-16), ymax(16), zmin(-16), zmax(16);

	// Lazy setup of cluster 1
	cout<<"Initalise:"<<endl;
	for(int i = 0; i<n/2; i++){	
		vector<vector<current_dtype> > temp = init( rands,{-10.,-10.,-10.},{0.06,0.02,0.02});
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// Lazy setup of cluster 2
	for(int i = n/2; i<n; i++){	
		vector<vector<current_dtype> > temp = init( rands,{10.,10.,10.},{-.07,-0.01,-0.02});
		for(int k = 0; k<3;k++){
			strided_pos[3*i+k] = temp[0][k];
			strided_vel[3*i+k] = temp[1][k];
		}
	}

	// Init leapfrom half step
	leapfrog_init_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
	
	cout<<"Initial totalE: " + to_string(totalE)<<endl;
	write_state(strided_pos, to_string(file_n)+"_pos");
	double wt = get_wall_time();



	GLFWwindow* window;
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  window = glfwCreateWindow(1000, 1000, "Look mah!", NULL, NULL);
  // glfwSetKeyCallback(window, key_callback);

  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
 if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}  

  std::cout << glGetString(GL_VERSION) << std::endl;
  //double step_x(0.), step_y(0.), theta(0.);

  float positions[] = {
   -0.5f, -0.5f,
    0.5f, -0.5f,
    0.5f, 0.5f,
   -0.5f, 0.5f,
  };

  unsigned int indicies[] = {
    0,1,2,
    2,3,0
  };
  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  unsigned int vao; // VAO stores layout of VBO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int buffer; // defines buffer label/index for identifying different buffers we can bind (point/pass to) GPU buffers
  glGenBuffers(1, &buffer);// make a single buffer and assign it to the buffer pointer/label
  glBindBuffer(GL_ARRAY_BUFFER, buffer); // note that there is only a GL_ARRAY_BUFFER on GPU, consider it a global we can have different values for
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW); // from docs: 'creates and inits buffer objects data store' 
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); // final arg is void pointer but can cast integer to that, use offset of for vertex struct THIS WRITES TO VAO basically the linker between VAO and VBO, stores reference to VBO and layout in VAO
 
  unsigned int ibo;  //  index buffer allows us to reuse vertices in multiple triangles
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW); 
  

  ShaderProgramSource ShaderSource = ImportShader("res/shaders/basic.shader");
  unsigned int shader = CreateShader(ShaderSource.VertexSource, ShaderSource.FragmentSource);
  glUseProgram(shader);
      const char* glsl_version = "#version 130";

  ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

  

  while(!glfwWindowShouldClose(window)){



  // while(t<tmax){
  
   leapfrog_step_strided(strided_pos, strided_vel, strided_force, dt, n, totalE, strided_force_threadcpy) ;
   if(step%10==0){
     // write_state(strided_pos, to_string(file_n)+"_pos");
     // file_n++;
    ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

         if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            double x = (t/tmax);
            ImGui::Text("Progress %.3f" , x );               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
           ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    glfwPollEvents();
   }
   t += dt;
   step++;
   progress( t/tmax, totalE );
  // }
      // Start the Dear ImGui frame
        

  	// glClear(GL_COLOR_BUFFER_BIT);
    // glDrawArrays(GL_TRIANGLES, 0, 6); // already have buffer bound so this is the state machine aspect
  	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // number of indicies, nullptr because buffer already bound 

      // Rendering
     
  	// this_thread::sleep_for(chrono::milliseconds(50));
  }

  // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  glfwTerminate();

	 // ImGui::CreateContext();
  //    ImGui_ImplGlfw_InitForOpenGL(window, true);
  //    ImGui::StyleColorsDark();

  //    ImGui_ImplOpenGL3_NewFrame();
  //       ImGui_ImplGlfw_NewFrame();
  //       ImGui::NewFrame();



	// cout<<"\n"<<endl;
	// double t_total = ( get_wall_time() - wt );
	// cout<< "Total time: " <<setprecision(3) << t_total <<"s"<<endl;
	// cout<< "FPS: "<< setprecision(2) << (double) step/t_total <<endl;
	// cout<<setprecision(32)<<strided_pos[0]<<"\t"<<strided_pos[10]<<endl;
	// cout<<setprecision(15)<<strided_force_threadcpy[0][0]<<"\t"<<strided_force_threadcpy[1][0]<<"\t"<<strided_force_threadcpy[2][0]<<endl;
	return 0;
}
