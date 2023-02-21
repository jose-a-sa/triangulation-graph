#include "TriangulationFlipGraph.hpp"

#include <stack>

TriangulationFlipGraph::TriangulationFlipGraph(const std::vector<Point>& pts)
{
    MeshTriangulation parentMesh(pts);
    parentMesh.triangulate();
    vertices_.push_back(std::move(parentMesh));
}

TriangulationFlipGraph::TriangulationFlipGraph(std::vector<Point>&& pts)
{
    MeshTriangulation parentMesh(std::move(pts));
    parentMesh.triangulate();
    vertices_.push_back(std::move(parentMesh));
}

void TriangulationFlipGraph::generateGraph()
{
    if(vertices_.size() != 1)
        return;

    indexMap_.clear();

    boost::hash<MeshTriangulation> hasher{};

    std::stack<MeshTriangulation> bfs;
    bfs.push(vertices_[0]);
    indexMap_[hasher(vertices_[0])] = 0;

    while(!bfs.empty())
    {
        MeshTriangulation curr = bfs.top();
        bfs.pop();
        std::size_t currHash = hasher(curr);

        for(auto [l, lf] : curr.flippableLines())
        {
            MeshTriangulation flipped(curr);
            flipped.flipLine(l);
            std::size_t flippedHash = hasher(flipped);

            if(!indexMap_.count(flippedHash))
            {
                bfs.push(flipped);
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

const std::unordered_set<std::pair<std::size_t, std::size_t>>& TriangulationFlipGraph::edges() const
{
    return edges_;
}
