all:
	g++ -std=c++11 main.cpp triangulation.cpp dcel.cpp -o triangulation -O3 -DNDBUG