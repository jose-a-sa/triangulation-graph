#ifndef CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
#define CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP

#include <boost/functional/hash.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "MeshTriangulation.hpp"

template<>
struct std::hash<std::pair<std::size_t, std::size_t>>
{
	std::size_t operator()(const std::pair<std::size_t, std::size_t>& p) const noexcept
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, p.first);
		boost::hash_combine(seed, p.second);
		return seed;
	}
};

class TriangulationFlipGraph
{
public:
	explicit TriangulationFlipGraph(const std::vector<Point>& pts);
	explicit TriangulationFlipGraph(std::vector<Point>&& pts);
	void generateGraph();
	const std::vector<MeshTriangulation>& vertices() const;
	const std::unordered_set<std::pair<std::size_t, std::size_t>>& edges() const;
private:
	std::vector<MeshTriangulation> vertices_;
	std::unordered_map<std::size_t, std::size_t> indexMap_;
	std::unordered_set<std::pair<std::size_t, std::size_t>> edges_;
};

#endif //CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
