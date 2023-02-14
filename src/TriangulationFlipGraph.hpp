#ifndef CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
#define CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "MeshTriangulation.hpp"

class TriangulationFlipGraph
{
public:
	explicit TriangulationFlipGraph(const std::vector<Point<int, 2>>& pts);
	explicit TriangulationFlipGraph(std::vector<Point<int, 2>>&& pts);
	void generateGraph();
	const std::unordered_set<MeshTriangulation>& vertices() const;
private:
	MeshTriangulation parentMesh_;
	std::unordered_set<MeshTriangulation> vertices_;
	std::unordered_map<MeshTriangulation, std::unordered_set<MeshTriangulation>> adj_;
	std::vector<std::pair<std::size_t, std::size_t>> edges_;
};

#endif //CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
