CXXFLAGS=-Wall -Werror -std=c++17 -pedantic -ggdb
LIBS=

de3df: main.cpp
	$(CXX) $(CXXFLAGS) -o de3df main.cpp $(LIBS)
