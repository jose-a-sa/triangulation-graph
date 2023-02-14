#include "TriangulationFlipGraph.hpp"

#include <stack>

TriangulationFlipGraph::TriangulationFlipGraph(const std::vector<Point<int, 2>>& pts)
	: parentMesh_(pts)
{
}

TriangulationFlipGraph::TriangulationFlipGraph(std::vector<Point<int, 2>>&& pts)
	: parentMesh_(std::move(pts))
{
}

void TriangulationFlipGraph::generateGraph()
{
	parentMesh_.triangulateDelaunay();
	vertices_.insert(std::move(parentMesh_));

	std::stack<MeshTriangulation> bfs;
	bfs.push(parentMesh_);

	const std::size_t MAX_ITER = 10;
	std::size_t i = 0;

	while (i++ < MAX_ITER && !bfs.empty())
	{
		std::cout << std::string(120, '-') << '\n';
		std::size_t n = bfs.size();
		while (n--)
		{
			const MeshTriangulation &curr = bfs.top();

			std::cout << &curr << ": \t" << curr.wkt() << " \t";
			std::cout << boost::hash<MeshTriangulation>()(curr) << '\n';

			for (const auto& [l,lf] : curr.flippableLines())
			{
				MeshTriangulation currFlipped(curr);
				currFlipped.flipEdge(l);

				auto foundMesh_it = vertices_.find(currFlipped);

				if (foundMesh_it != vertices_.end())
				{
					const MeshTriangulation& foundMesh = *foundMesh_it;
					adj_[curr].insert(foundMesh);
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

const std::unordered_set<MeshTriangulation>& TriangulationFlipGraph::vertices() const
{
	return vertices_;
}
