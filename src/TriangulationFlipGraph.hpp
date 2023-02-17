#ifndef CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
#define CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "MeshTriangulation.hpp"

class TriangulationFlipGraph
{
public:
	explicit TriangulationFlipGraph(const std::vector<Point<double, 2>>& pts);
	explicit TriangulationFlipGraph(std::vector<Point<double, 2>>&& pts);
	void generateGraph();
	const std::vector<MeshTriangulation>& vertices() const;
	const std::unordered_set<Point<std::size_t, 2>>& edges() const;
private:
	std::vector<MeshTriangulation> vertices_;
	std::unordered_set<std::size_t> visitedHash_;
	std::unordered_map<std::size_t, std::size_t> indexMap_;
	std::unordered_set<Point<std::size_t, 2>> edges_;
};

#endif //CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
