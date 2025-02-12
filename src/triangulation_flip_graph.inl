#ifndef CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_INL
#define CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_INL


#include "moodycamel/concurrent_queue.h"


template<class Traits>
void TriangulationFlipGraph<Traits>::generate_graph()
{
    nodes_.clear();
    edges_.clear();

    // get seed triangulation from MeshTriangulation2D
    auto seed = std::make_unique<Mesh_t>(coords_ptr_);
    seed->triangulate();

    std::queue<Mesh_t const*> bfs_queue_;

    // push seed into a lock-free queue
    auto const insert_res0 = nodes_.insert(std::move(seed));
    if(insert_res0.second)
        bfs_queue_.push(insert_res0.first->get());

    while(!bfs_queue_.empty())
    {
        auto const current_triangulation = bfs_queue_.front();
        bfs_queue_.pop();

        // fmt::println("Got: {}", current_triangulation->get_edge_adjacency().size());

        for(auto const& edge: current_triangulation->flippable())
        {
            auto new_triangulation_ptr = std::make_unique<Mesh_t>(*current_triangulation);
            new_triangulation_ptr->flip_edge(edge);

            auto const insert_res = nodes_.insert(std::move(new_triangulation_ptr));
            edges_.emplace(current_triangulation, insert_res.first->get());
            if(insert_res.second)
            {
                bfs_queue_.push(insert_res.first->get());
            }
        }
        // current_triangulation = nullptr;
    }
}


template<class Traits>
void ConcurrentTriangulationFlipGraph<Traits>::generate_graph(size_t const num_threads)
{
    nodes_.clear();
    edges_.clear();

    nodes_.reserve(1 << (8 + coords_ptr_->size()));
    edges_.reserve(1 << (8 + coords_ptr_->size()));


    struct BFSState
    {
        EdgeCell      edge{};
        Mesh_t const* triangulation{};

        BFSState() = default;
        BFSState(EdgeCell const eg, Mesh_t const* tr)
            : edge(eg),
              triangulation(tr)
        {}
    };
    moodycamel::ConcurrentQueue<BFSState> bfs_queue_;

    // get seed triangulation from MeshTriangulation2D
    auto seed = std::make_unique<Mesh_t>(coords_ptr_);
    seed->triangulate();
    Mesh_t const* current_triangulation = nodes_.insert(std::move(seed)).first->get();

    for(auto&& eg: current_triangulation->flippable())
        bfs_queue_.enqueue(BFSState(eg, current_triangulation));


    // constexpr size_t NUM_THREADS = 8;

    auto worker = [&](size_t)
    {
        // size_t counter = 0;
        BFSState state{};
        while(bfs_queue_.try_dequeue(state))
        {
            auto new_triangulation_ptr = std::make_unique<Mesh_t>(*state.triangulation);
            new_triangulation_ptr->flip_edge(state.edge);

            auto const [ptr, inserted] = nodes_.emplace(std::move(new_triangulation_ptr));
            edges_.emplace(state.triangulation, ptr->get());

            if(inserted)
            {
                for(auto&& eg: ptr->get()->flippable())
                    bfs_queue_.enqueue(BFSState(eg, ptr->get()));
            }
            // ++counter;
        }
        // spdlog::info("Thread {} finished with {} flips", idx, counter);
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for(size_t t = 0; t < num_threads; ++t)
    {
        threads.emplace_back(worker, t);
    }
    // worker(num_threads);

    for(auto& thread: threads)
    {
        if(thread.joinable())
            thread.join();
    }
}

#endif // CONVEX_TRIANGULATIONS_TRIANGULATIONFLIPGRAPH_INL
