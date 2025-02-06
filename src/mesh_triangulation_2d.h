#ifndef CONVEX_TRIANGULATIONS_MESHTRIANGULATION2D_HPP
#define CONVEX_TRIANGULATIONS_MESHTRIANGULATION2D_HPP

#include "mesh_cell.h"
#include "utils/hash.h"
#include "vec2.h"

#include <cstddef>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>


#define MESH_TRIANGULATION_HAS_STD 1
#define MESH_TRIANGULATION_HAS_TBB 1
#define MESH_TRIANGULATION_HAS_ABSL 1


struct EdgeCellHash
{
    constexpr size_t operator()(EdgeCell const& eg) const noexcept { return HashUtils::combine(eg.a, eg.b); }
};


struct MeshTriangulationDefaultTraits
{
    template<class... Args>
    using unordered_set_t = std::unordered_set<Args...>;
    template<class... Args>
    using unordered_map_t = std::unordered_map<Args...>;
    
    template<class Map, class T>
    static constexpr auto erase(Map&& map, T&& v)
    {
        return std::forward<Map>(map).erase(std::forward<T>(v));
    }
};


#if MESH_TRIANGULATION_HAS_TBB
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>

struct MeshTriangulationTbbTraits
{
    template<class... Args>
    using unordered_set_t = tbb::concurrent_unordered_set<Args...>;
    template<class... Args>
    using unordered_map_t = tbb::concurrent_unordered_map<Args...>;

    template<class Map, class T>
    static constexpr auto erase(Map&& map, T&& v)
    {
        return std::forward<Map>(map).unsafe_erase(std::forward<T>(v));
    }
};
#endif


#if MESH_TRIANGULATION_HAS_ABSL
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

struct MeshTriangulationAbslTraits
{
    template<class... Args>
    using unordered_set_t = absl::flat_hash_set<Args...>;
    template<class... Args>
    using unordered_map_t = absl::flat_hash_map<Args...>;
    
    template<class Map, class T>
    static constexpr auto erase(Map&& map, T&& v)
    {
        return std::forward<Map>(map).erase(std::forward<T>(v));
    }
};
#endif



template<class Traits = MeshTriangulationDefaultTraits>
class MeshTriangulation2D
{
public:
    enum class Result
    {
        Success,
        FailedEdgeNotFound,
        FailedFlipBorderEdge,
        FailedNotEnoughPoints,
        FailedSweepHullSort,
        FailedUpdateAdjacency
    };

    using float_type      = Vec2::float_type;
    using traits          = Traits;
    using unordered_set_t = typename Traits::template unordered_set_t<EdgeCell, EdgeCellHash>;
    using unordered_map_t = typename Traits::template unordered_map_t<EdgeCell, EdgeCell, EdgeCellHash>;

    explicit MeshTriangulation2D(std::shared_ptr<std::vector<Vec2> const> pts)
        : coords_ptr_(std::move(pts)) {};

    Result triangulate();
    Result flip_edge(EdgeCell edge);

    unordered_map_t const& edge_adjacency() const { return edge_adjacency_; }
    unordered_set_t const& flippable() const { return flippable_; }

private:
    unordered_map_t                          edge_adjacency_;
    unordered_set_t                          flippable_;
    std::shared_ptr<std::vector<Vec2> const> coords_ptr_;

    bool sweep_hull_sort(std::vector<size_t>& idxs, size_t start = 0) const;
    void sweep_hull_add(std::vector<size_t>& hull, std::vector<size_t> const& idxs);
    void insert_or_update_adjacency(TriangleCell const& t);
    bool replace_adjacency(EdgeCell const& edge, size_t from, size_t to);

    bool       is_convex_polygon(EdgeCell const& edge, EdgeCell const& opposite) const;
    float_type triangle_area(size_t a, size_t b, size_t c) const;
};

// include inline implementation details
#include "mesh_triangulation_2d.inl"

#endif // CONVEX_TRIANGULATIONS_MESHTRIANGULATION2D_HPP
