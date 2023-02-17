#include "TriangulationFlipGraph.hpp"

#include <stack>

TriangulationFlipGraph::TriangulationFlipGraph(const std::vector<Point<double, 2>>& pts)
{
	MeshTriangulation parentMesh(pts);
	parentMesh.triangulateDelaunay();
	vertices_.push_back(std::move(parentMesh));
}

TriangulationFlipGraph::TriangulationFlipGraph(std::vector<Point<double, 2>>&& pts)
{
	MeshTriangulation parentMesh(std::move(pts));
	parentMesh.triangulateDelaunay();
	vertices_.push_back(std::move(parentMesh));
}

void TriangulationFlipGraph::generateGraph()
{
	if (vertices_.size() != 1)
		return;

	visitedHash_.clear();
	indexMap_.clear();

	boost::hash<MeshTriangulation> hasher{};

	std::stack<MeshTriangulation> bfs;
	bfs.push(vertices_[0]);
	std::size_t parentHash = hasher(vertices_[0]);
	visitedHash_.insert(parentHash);
	indexMap_[parentHash] = 0;

	while (!bfs.empty())
	{
		MeshTriangulation curr = bfs.top();
		bfs.pop();
		std::size_t currHash = hasher(curr);

		for (const auto& [l, lf] : curr.flippableLines())
		{
			MeshTriangulation flipped(curr);
			flipped.flipEdge(l);
			std::size_t flippedHash = hasher(flipped);

			if (!visitedHash_.count(flippedHash))
			{
				bfs.push(flipped);
				visitedHash_.insert(flippedHash);
				vertices_.push_back(std::move(flipped));
				indexMap_[flippedHash] = vertices_.size() - 1;
			}

			edges_.insert({indexMap_[currHash], indexMap_[flippedHash]});
		}
	}
}

const std::vector<MeshTriangulation>& TriangulationFlipGraph::vertices() const
{
	return vertices_;
}

const std::unordered_set<Point<std::size_t, 2>>& TriangulationFlipGraph::edges() const
{
	return edges_;
}
