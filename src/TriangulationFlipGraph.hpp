#ifndef CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
#define CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP

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
    void generateGraph();
    [[nodiscard]] const std::vector<MeshTriangulation>& vertices() const;
    [[nodiscard]] const std::unordered_set<std::pair<std::size_t, std::size_t>>& edges() const;
#ifndef GTEST_NAME

    friend class TriangulationFlipGraphFixture;

#endif
protected:
    void init();
private:
    std::shared_ptr<std::vector<Point>> mp_coords;
    std::vector<MeshTriangulation> m_vertices;
    std::unordered_map<std::size_t, std::size_t> m_indexMap;
    std::unordered_set<std::pair<std::size_t, std::size_t>> m_edges;
};

#endif //CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
