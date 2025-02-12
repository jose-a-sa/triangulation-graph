#ifndef CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
#define CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP

#include "mesh_triangulation_2d.h"
#include "utils/hash.h"
#include "vec2.h"

#include <cstddef>

#include <moodycamel/concurrent_queue.h>
#include <tbb/concurrent_unordered_set.h>
#include <unordered_set>
#include <vector>


struct MeshTriangulation2DCRefHash
{
    template<class MeshTraits>
    constexpr size_t operator()(std::unique_ptr<MeshTriangulation2D<MeshTraits> const> const& trig) const noexcept
    {
        return HashUtils::combine_unordered_range(trig->flippable().begin(), trig->flippable().end(), EdgeCellHash{});
    }
};

struct MeshTriangulation2DCRefEqualTo
{
    template<class MeshTraits>
    constexpr size_t operator()(std::unique_ptr<MeshTriangulation2D<MeshTraits> const> const& lhs,
                                std::unique_ptr<MeshTriangulation2D<MeshTraits> const> const& rhs) const noexcept
    {
        return lhs->flippable() == rhs->flippable();
    }
};

struct MeshTriangulation2DCRefEdgeHash
{
    template<class MeshTraits>
    constexpr size_t
    operator()(std::pair<MeshTriangulation2D<MeshTraits> const*, MeshTriangulation2D<MeshTraits> const*> const& p)
        const noexcept
    {
        return HashUtils::combine_unordered_pair(p, std::hash<MeshTriangulation2D<MeshTraits> const*>{});
    }
};


template<class MeshTraits>
class TriangulationFlipGraph
{
    using Mesh_t = MeshTriangulation2D<MeshTraits>;
    using NodeSet_t =
        std::unordered_set<std::unique_ptr<Mesh_t const>, MeshTriangulation2DCRefHash, MeshTriangulation2DCRefEqualTo>;
    using TrigEdgeSet_t = std::unordered_set<std::pair<Mesh_t const*, Mesh_t const*>, MeshTriangulation2DCRefEdgeHash>;

public:
    explicit TriangulationFlipGraph(std::shared_ptr<std::vector<Vec2> const> pts)
        : coords_ptr_(std::move(pts))
    {}

    void generate_graph();

    NodeSet_t const&     nodes() const { return nodes_; }
    TrigEdgeSet_t const& edges() const { return edges_; }

private:
    std::shared_ptr<std::vector<Vec2> const> coords_ptr_;
    NodeSet_t                                nodes_;
    TrigEdgeSet_t                            edges_;
};


template<class MeshTraits>
class ConcurrentTriangulationFlipGraph
{
    using Mesh_t    = MeshTriangulation2D<MeshTraits>;
    using NodeSet_t = tbb::concurrent_unordered_set<std::unique_ptr<Mesh_t const>, MeshTriangulation2DCRefHash,
                                                    MeshTriangulation2DCRefEqualTo>;
    using TrigEdgeSet_t =
        tbb::concurrent_unordered_set<std::pair<Mesh_t const*, Mesh_t const*>, MeshTriangulation2DCRefEdgeHash>;

public:
    explicit ConcurrentTriangulationFlipGraph(std::shared_ptr<std::vector<Vec2> const> pts)
        : coords_ptr_(std::move(pts))
    {}

    void generate_graph(size_t num_threads = 8);

    NodeSet_t const&     nodes() const { return nodes_; }
    TrigEdgeSet_t const& edges() const { return edges_; }

private:
    std::shared_ptr<std::vector<Vec2> const> coords_ptr_;
    NodeSet_t                                nodes_;
    TrigEdgeSet_t                            edges_;
};

// template<class MeshTraits>
// class ConcurrentTriangulationFlipGraphCDS
// {
//     using Mesh_t    = MeshTriangulation2D<MeshTraits>;
//     using NodeSet_t = tbb::concurrent_unordered_set<std::unique_ptr<Mesh_t const>, MeshTriangulation2DCRefHash,
//                                                     MeshTriangulation2DCRefEqualTo>;
//     using TrigEdgeSet_t =
//         tbb::concurrent_unordered_set<std::pair<Mesh_t const*, Mesh_t const*>, MeshTriangulation2DCRefEdgeHash>;
//
// public:
//     explicit ConcurrentTriangulationFlipGraphCDS(std::shared_ptr<std::vector<Vec2> const> pts)
//         : coords_ptr_(std::move(pts))
//     {}
//
//     void generate_graph(size_t num_threads = 8);
//
//     NodeSet_t const&     nodes() const { return nodes_; }
//     TrigEdgeSet_t const& edges() const { return edges_; }
//
// private:
//     std::shared_ptr<std::vector<Vec2> const> coords_ptr_;
//     NodeSet_t                                nodes_;
//     TrigEdgeSet_t                            edges_;
//
//
//     struct BFSState
//     {
//         EdgeCell      edge{};
//         Mesh_t const* triangulation{};
//
//         BFSState() = default;
//         BFSState(EdgeCell const eg, Mesh_t const* tr)
//             : edge(eg),
//               triangulation(tr)
//         {}
//     };
//     moodycamel::ConcurrentQueue<BFSState> bfs_queue_;
// };


#include "triangulation_flip_graph.inl"


#endif // CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_HPP
