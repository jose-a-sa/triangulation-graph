#include <iostream>
#include <vector>
#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"

int main()
{
	std::vector<Point<int, 2>> pts{{ 0, 0 },
								   { 1, 0 },
								   { 0, 1 },
								   { 1, 1 },
								   { 1, 2 },
								   { 0, 2 }}; // points owned by MeshTriangulation

	std::cout << "Points created !" << std::endl;

	TriangulationFlipGraph gr(pts);
	gr.generateGraph();

	std::cout << std::endl;


	for(const auto& mesh : gr.vertices())
		std::cout << mesh.wkt() << std::endl;

	return 0;
}