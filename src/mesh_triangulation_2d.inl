#ifndef MESH_TRIANGULATION_2D_INL
#define MESH_TRIANGULATION_2D_INL

#include <vector>
#include "mesh_triangulation_2d.h"
#include "utils/algorithm.h"

namespace detail
{
    template<class T, class U>
    bool is_same_epsilon(T a, U b)
    {
        using diff_t = std::common_type_t<T, U>;
        diff_t const max_epsilon =
            std::max<diff_t>(std::numeric_limits<T>::epsilon(), std::numeric_limits<U>::epsilon());
        diff_t const diff = std::abs(a - b);
        return diff < 3 * max_epsilon;
    }

    template<class T>
    struct VectorCircularHelper
    {
        using iterator = typename std::vector<T>::iterator;

        explicit VectorCircularHelper(std::vector<T>& vec)
            : vec_(vec) {};

        iterator next(iterator it) const noexcept { return std::next(it) == vec_.end() ? vec_.begin() : std::next(it); }
        iterator prev(iterator it) const noexcept { return it == vec_.begin() ? std::prev(vec_.end()) : std::prev(it); }

        void replace_range(iterator begin, iterator end, T const& value)
        {
            if(begin == end)
            {
                vec_.insert(end == vec_.begin() ? vec_.end() : end, value);
            }
            else if(begin < end)
            {
                *begin = value;
                vec_.erase(std::next(begin), end);
            }
            else
            {
                vec_.erase(begin, vec_.end());
                if(end != vec_.begin())
                    vec_.erase(vec_.begin(), end);
                vec_.insert(vec_.end(), value);
            }
        }

    private:
        std::vector<T>& vec_;
    };
} // namespace detail


template<class Traits>
inline typename MeshTriangulation2D<Traits>::Result MeshTriangulation2D<Traits>::flip_edge(EdgeCell edge)
{
    auto const it = edge_adjacency_.find(edge);
    if(it == edge_adjacency_.end())
        return Result::FailedEdgeNotFound;

    auto const flip_it = flippable_.find(edge);
    if(flip_it == flippable_.end())
        return Result::FailedFlipBorderEdge;

    auto const opposite = it->second;

    traits::erase(edge_adjacency_, it);
    edge_adjacency_.emplace(opposite, edge);

    traits::erase(flippable_, flip_it);
    flippable_.emplace(opposite);

    bool succ = true;
    succ &= replace_adjacency(EdgeCell(edge.a, opposite.a), edge.b, opposite.b);
    succ &= replace_adjacency(EdgeCell(edge.a, opposite.b), edge.b, opposite.a);
    succ &= replace_adjacency(EdgeCell(edge.b, opposite.a), edge.a, opposite.b);
    succ &= replace_adjacency(EdgeCell(edge.b, opposite.b), edge.a, opposite.a);

    return succ ? Result::Success : Result::FailedUpdateAdjacency;
}


template<class Traits>
inline bool MeshTriangulation2D<Traits>::replace_adjacency(EdgeCell const& edge, size_t from_idx, size_t to_idx)
{
    auto const it = edge_adjacency_.find(edge);
    if(it == edge_adjacency_.end())
        return false;

    auto const old_opposite = it->second;

    if(old_opposite.a == from_idx)
        it->second = EdgeCell(to_idx, old_opposite.b);
    else if(old_opposite.b == from_idx)
        it->second = EdgeCell(old_opposite.a, to_idx);
    else // not found
        return false;

    auto const new_opposite = it->second;
    if(new_opposite.undefined() || !is_convex_polygon(edge, new_opposite))
        traits::erase(flippable_, edge);
    else
        flippable_.emplace(edge);

    return true;
}


template<class Traits>
inline typename MeshTriangulation2D<Traits>::float_type MeshTriangulation2D<Traits>::triangle_area(size_t a, size_t b,
                                                                                                   size_t c) const
{
    assert(coords_ptr_);
    auto const& coords = *coords_ptr_;

    float_type const ab_norm2      = (coords[a] - coords[b]).norm2();
    float_type const ac_norm2      = (coords[a] - coords[c]).norm2();
    float_type const bc_norm2      = (coords[b] - coords[c]).norm2();
    float_type const ac2_minus_bc2 = ac_norm2 - bc_norm2;

    float_type const triangle_area2 =
        0.5 * (-ab_norm2 * ab_norm2 - ac2_minus_bc2 * ac2_minus_bc2 + 2.0 * ab_norm2 * (ac_norm2 + bc_norm2));

    return std::sqrt(triangle_area2);
}


template<class Traits>
inline bool MeshTriangulation2D<Traits>::is_convex_polygon(EdgeCell const& edge, EdgeCell const& opposite) const
{
    float_type const t1   = triangle_area(edge.a, edge.b, opposite.a);
    float_type const t2   = triangle_area(edge.a, edge.b, opposite.b);
    float_type const t1_f = triangle_area(opposite.a, opposite.b, edge.a);
    float_type const t2_f = triangle_area(opposite.a, opposite.b, edge.b);

    return detail::is_same_epsilon(t1 + t2, t1_f + t2_f) && !detail::is_same_epsilon(t1_f, 0) &&
           !detail::is_same_epsilon(t2_f, 0);
}


template<class Traits>
inline typename MeshTriangulation2D<Traits>::Result MeshTriangulation2D<Traits>::triangulate()
{
    edge_adjacency_.clear();

    assert(coords_ptr_);
    auto const&  coords      = *coords_ptr_;
    size_t const coords_size = coords.size();
    if(coords_size < 3)
        return Result::FailedNotEnoughPoints;

    std::vector<size_t> idxs(coords_size);
    std::iota(idxs.begin(), idxs.end(), 0);

    if(!sweep_hull_sort(idxs, 0))
        return Result::FailedSweepHullSort;

    std::vector<size_t> hull{};
    hull.reserve(coords_size);
    sweep_hull_add(hull, idxs);

    return Result::Success;
}


template<class Traits>
inline bool MeshTriangulation2D<Traits>::sweep_hull_sort(std::vector<size_t>& idxs, size_t start) const
{
    assert(coords_ptr_);
    auto const&  coords = *coords_ptr_;
    size_t const n      = idxs.size();

    size_t pivot = start;
    while(pivot < n + start)
    {
        std::swap(idxs[0], idxs[pivot % n]);
        auto const& pivot_pt = coords.front();

        auto const closest_it = min_element_by(std::next(idxs.begin(), 1), idxs.end(),
                                               [&](size_t const idx) { return pivot_pt.distance(coords[idx]); });
        std::swap(idxs[1], *closest_it);

        auto const min_circumcircle_it =
            min_element_by(std::next(idxs.begin(), 2), idxs.end(), [&](size_t const idx)
                           { return pivot_pt.circumcircle(coords[idxs[1]], coords[idx]).second; });
        if(min_circumcircle_it == idxs.end())
        {
            ++pivot;
            continue;
        }
        std::swap(idxs[2], *min_circumcircle_it);

        float_type const cross = Vec2::cross(pivot_pt, coords[idxs[1]], coords[idxs[2]]);
        if(detail::is_same_epsilon(cross, 0))
        {
            ++pivot;
            continue;
        }
        if(cross < 0)
            std::swap(idxs[1], idxs[2]);

        auto const circumcircle_pair = pivot_pt.circumcircle(coords[idxs[1]], coords[idxs[2]]);
        if(std::isnan(circumcircle_pair.second))
        {
            ++pivot;
            continue;
        }

        if(n > 4)
        {
            Vec2 const& center = circumcircle_pair.first;
            std::sort(std::next(idxs.begin(), 3), idxs.end(), [&](size_t const i, size_t const j)
                      { return center.distance(coords[i]) < center.distance(coords[j]); });
        }

        return true;
    }

    return false;
}


template<class Traits>
inline void MeshTriangulation2D<Traits>::sweep_hull_add(std::vector<size_t>& hull, std::vector<size_t> const& idxs)
{
    assert(coords_ptr_);
    auto const& coords = *coords_ptr_;

    assert(idxs.size() == coords.size());
    assert(idxs.size() >= 3);

    detail::VectorCircularHelper helper(hull);

    hull.insert(hull.end(), idxs.begin(), std::next(idxs.begin(), 3));
    insert_or_update_adjacency(TriangleCell(idxs[0], idxs[1], idxs[2]));

    std::for_each(std::next(idxs.begin(), 3), idxs.end(),
                  [&](size_t const idx)
                  {
                      auto cross_test = [&](std::vector<size_t>::iterator const it)
                      { return Vec2::cross(coords[idx], coords[*it], coords[*helper.next(it)]) < 0; };

                      auto begin = hull.begin();
                      while(cross_test(helper.prev(begin)) || !cross_test(begin))
                      {
                          begin = helper.next(begin);
                      }

                      auto end = begin;
                      while(!cross_test(helper.prev(end)) || cross_test(end))
                      {
                          end = helper.next(end);
                          insert_or_update_adjacency(TriangleCell(idx, *helper.prev(end), *end));
                      }

                      helper.replace_range(helper.next(begin), end, idx);
                  });
}


template<class Traits>
inline void MeshTriangulation2D<Traits>::insert_or_update_adjacency(TriangleCell const& t)
{
    std::array<std::pair<EdgeCell, size_t>, 3> const pairs{std::make_pair(EdgeCell(t.a, t.b), t.c),
                                                           std::make_pair(EdgeCell(t.a, t.c), t.b),
                                                           std::make_pair(EdgeCell(t.b, t.c), t.a)};
    for(auto&& eg_op_pair: pairs)
    {
        auto&& eg           = eg_op_pair.first;
        auto&& opposite_idx = eg_op_pair.second;

        auto const it = edge_adjacency_.find(eg);
        if(it == edge_adjacency_.end())
        {
            edge_adjacency_.emplace(eg, EdgeCell(opposite_idx, -1ull));
            continue;
        }

        auto& opposite_eg = it->second;
        if(opposite_eg.undefined())
        {
            opposite_eg = EdgeCell(opposite_eg.a, opposite_idx);
            if(is_convex_polygon(eg, opposite_eg))
                flippable_.emplace(eg);
        }
        else
        {
            // remove exception
            throw std::runtime_error(fmt::format("MeshTriangulation2D::insert_or_update_adjacency, adding edge {} that "
                                                 "already has two adjacent triangles {}.",
                                                 eg, opposite_eg));
        }
    }
}

#endif // MESH_TRIANGULATION_2D_INL
