#include "TriangulationFlipGraph.hpp"

#include <queue>

TriangulationFlipGraph::TriangulationFlipGraph(const std::vector<Point>& pts)
{
    m_vertices.emplace_back(pts);
    m_vertices[0].triangulate();
}

TriangulationFlipGraph::TriangulationFlipGraph(std::vector<Point>&& pts)
{
    m_vertices.emplace_back(std::move(pts));
    m_vertices[0].triangulate();;
}

void TriangulationFlipGraph::generateGraph()
{
    if(m_vertices.size() != 1)
        return;

    m_indexMap.clear();
    m_edges.clear();

    boost::hash<MeshTriangulation> hasher{};

    std::queue<std::size_t> bfs;
    bfs.push(0);
    m_indexMap[hasher(m_vertices[0])] = 0;

    while(!bfs.empty())
    {
        std::size_t currIdx = bfs.front();
        bfs.pop();
        std::size_t currHash = hasher(m_vertices[currIdx]);

        auto flippable = m_vertices[currIdx].flippableLines();
        for(auto& kv : flippable)
        {
            MeshTriangulation flipped(m_vertices[currIdx]);
            flipped.flipLine(kv.first);
            std::size_t flippedHash = hasher(flipped);

            if(!m_indexMap.count(flippedHash))
            {
                m_vertices.push_back(std::move(flipped));
                bfs.push(m_vertices.size() - 1);
                m_indexMap[flippedHash] = m_vertices.size() - 1;
            }

            m_edges.insert({m_indexMap[currHash], m_indexMap[flippedHash]});
        }
    }
}

const std::vector<MeshTriangulation>& TriangulationFlipGraph::vertices() const
{
    return m_vertices;
}

const std::unordered_set<std::pair<std::size_t, std::size_t>>& TriangulationFlipGraph::edges() const
{
    return m_edges;
}
