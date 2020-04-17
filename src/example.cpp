#include "include/glad.h"
#include <GLFW/glfw3.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include <stdlib.h>
#include <iostream>
#include <chrono>        
#include <thread>
#include <fstream>
#include <sstream>
#include <unordered_map>
//using namespace std;

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


struct KeyState {
  KeyState(): press_left(false), press_right(false), press_up(false), press_down(false), rotate_left(false), rotate_right(false) {};
  bool press_left, press_right, press_up, press_down, rotate_left, rotate_right;
  
} ;

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
KeyState key_state;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      key_state.press_right = true;

    } else if(key == GLFW_KEY_D && action == GLFW_RELEASE){
      key_state.press_right = false;

    } 

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      key_state.press_left = true;

    } else if(key == GLFW_KEY_A && action == GLFW_RELEASE){
      key_state.press_left = false;
    } 

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      key_state.press_up = true;

    } else if(key == GLFW_KEY_W && action == GLFW_RELEASE){
      key_state.press_up = false;
    } 

     if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      key_state.press_down= true;

    } else if(key == GLFW_KEY_S && action == GLFW_RELEASE){
      key_state.press_down = false;
    } 

     if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      key_state.rotate_left= true;

    } else if(key == GLFW_KEY_Q && action == GLFW_RELEASE){
      key_state.rotate_left = false;
    } 

    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      key_state.rotate_right= true;

    } else if(key == GLFW_KEY_E && action == GLFW_RELEASE){
      key_state.rotate_right = false;
    } 
}

int main(void)
{
  GLFWwindow* window;
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  window = glfwCreateWindow(640, 480, "Look mah!", NULL, NULL);
  glfwSetKeyCallback(window, key_callback);

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
  

  ShaderProgramSource ShaderSource = ImportShader("src/res/shaders/basic.shader");
  unsigned int shader = CreateShader(ShaderSource.VertexSource, ShaderSource.FragmentSource);
  glUseProgram(shader);

  while(!glfwWindowShouldClose(window)){
  	glClear(GL_COLOR_BUFFER_BIT);
    // glDrawArrays(GL_TRIANGLES, 0, 6); // already have buffer bound so this is the state machine aspect
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // number of indicies, nullptr because buffer already bound 

    glfwSwapBuffers(window);

  	glfwPollEvents();
  	// this_thread::sleep_for(chrono::milliseconds(50));
  }

  glfwTerminate();
  return 0;
}




    // if(key_state.press_left){
    //   step_x -= 0.1;
    // } else if(key_state.press_right){
    //   step_x += 0.1;
    // } 
    
    // if(key_state.press_up){
    //   step_y += 0.1;
    // } else if(key_state.press_down){
    //   step_y -= 0.1;
    // }
   
    // if(key_state.rotate_left){
    //   theta += 0.1;
    // } else if(key_state.rotate_right){
    //   theta -= 0.1;
    // }
