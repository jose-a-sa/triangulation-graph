#include "TriangulationFlipGraph.hpp"

#include <stack>

TriangulationFlipGraph::TriangulationFlipGraph(const std::vector<Point<int, 2>>& pts)
	: parentMesh_(pts)
{
	parentMesh_.triangulateDelaunay();
	vertices_.insert(parentMesh_);
}

TriangulationFlipGraph::TriangulationFlipGraph(std::vector<Point<int, 2>>&& pts)
	: parentMesh_(std::move(pts))
{
	parentMesh_.triangulateDelaunay();
	vertices_.insert(parentMesh_);
}

void TriangulationFlipGraph::generateGraph()
{
	std::stack<MeshTriangulation> bfs;
	bfs.push(parentMesh_);

	while (!bfs.empty())
	{
		std::size_t levelSize = bfs.size();
		while (levelSize--)
		{
			const MeshTriangulation& curr = bfs.top();

			for (const auto& [l, lf] : curr.flippableLines())
			{
				MeshTriangulation currFlipped(curr);
				currFlipped.flipEdge(l);

				auto foundMesh_it = vertices_.find(currFlipped);

				if (foundMesh_it != vertices_.end())
				{
					adj_[curr].insert(*foundMesh_it);
					continue;
				}

				bfs.push(currFlipped);
				adj_[curr].insert(currFlipped);
				vertices_.insert(std::move(currFlipped));
			}

			bfs.pop();
		}
	}
}

const std::unordered_set<MeshTriangulation>&
TriangulationFlipGraph::vertices() const
{
	return vertices_;
}
