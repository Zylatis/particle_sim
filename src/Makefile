#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

#CXX = g++
#CXX = clang++

EXE = plummer_colission
SOURCES = plummer_colission.cpp
#SOURCES += ../imgui_impl_glfw.cpp ../imgui_impl_opengl3.cpp
#SOURCES += ../../imgui.cpp ../../imgui_demo.cpp ../../imgui_draw.cpp ../../imgui_widgets.cpp
#OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

#CXXFLAGS = -I../ -I../../
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## OPENGL LOADER
##---------------------------------------------------------------------

## Using OpenGL loader: gl3w [default]
#SOURCES += ../libs/gl3w/GL/gl3w.c
#CXXFLAGS += -I../libs/gl3w -DIMGUI_IMPL_OPENGL_LOADER_GL3W

## Using OpenGL loader: glew
## (This assumes a system-wide installation)
# CXXFLAGS += -lGLEW -DIMGUI_IMPL_OPENGL_LOADER_GLEW

## Using OpenGL loader: glad
# SOURCES += ../libs/glad/src/glad.c
# CXXFLAGS += -I../libs/glad/include -DIMGUI_IMPL_OPENGL_LOADER_GLAD

## Using OpenGL loader: glbinding
## This assumes a system-wide installation
## of either version 3.0.0 (or newer)
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING3
## or the older version 2.x
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib
	#LIBS += -lglfw3
	LIBS += -lglfw

	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
all:
	g++ plummer_collision.cpp -o plummer_collision  -std=c++11 include/imgui/imgui_impl_glfw.cpp  include/imgui/imgui_impl_opengl3.cpp  include/imgui/imgui_demo.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_widgets.cpp include/imgui/imgui.cpp include/glad.c  -I/home/graeme/coding_local/particle_sim/cpp/src/include/ -I/home/graeme/coding_local/particle_sim/cpp/src/include/ -lGL -lglfw -Ofast -std=c++11 -mtune=native -march=native -fopenmp -DIMGUI_IMPL_OPENGL_LOADER_GLAD

#%.o:%.cpp
#	$(CXX) $(CXXFLAGS) -c -o $@ $<

#%.o:../%.cpp
#	$(CXX) $(CXXFLAGS) -c -o $@ $<

#%.o:../../%.cpp
#	$(CXX) $(CXXFLAGS) -c -o $@ $<

#%.o:../libs/gl3w/GL/%.c
#	$(CC) $(CFLAGS) -c -o $@ $<

#%.o:../libs/glad/src/%.c
#	$(CC) $(CFLAGS) -c -o $@ $<

#all: $(EXE)
#	@echo Build complete for $(ECHO_MESSAGE)

#$(EXE): $(OBJS)
#	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

#clean:
#	rm -f $(EXE) $(OBJS)
