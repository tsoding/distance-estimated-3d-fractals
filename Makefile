CXXFLAGS=-O3 -Wall -Werror -std=c++17 -pedantic -ggdb
LIBS=
PKGS_GPU=glfw3 gl
CXXFLAGS_GPU=-O3 -Wall -Werror -std=c++17 -pedantic -ggdb `pkg-config --cflags $(PKGS_GPU)`
LIBS_GPU=`pkg-config --libs $(PKGS_GPU)`

all: cpu gpu

cpu: cpu.cpp
	$(CXX) $(CXXFLAGS) -o cpu cpu.cpp $(LIBS)

gpu: gpu.cpp
	$(CXX) $(CXXFLAGS_GPU) -o gpu gpu.cpp $(LIBS_GPU)
