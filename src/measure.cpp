#include <iostream>

#include "Utils.hpp"
#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"
#include <list>


int main(int argc, const char* argv[])
{
    RandomPoint randomPoint(-1.0, 1.0);
    std::vector<Point> pts = randomPoint.generate(10);

    for(const auto& pt : pts)
        std::cout << pt.toString() << " ";
    std::cout << "\n\n";

    std::list<std::size_t> lst{1, 2, 3, 4, 10, 20, 30, 40};
    lst.insert(lst.end(), 100);
    std::list<std::size_t>::iterator it = lst.begin();
    std::list<std::size_t>::iterator bb, ee;
    for(size_t i = 0; i < 35; i++)
    {
        if(*it == 40)
            bb = it;
        if(*it == 2)
            ee = it;
        std::cout << (it == lst.end() ? "[e]" : "");
        std::cout << (it == lst.begin() ? "[b]" : "");
        std::cout << *(it) << " ";
        it++;
    }
    std::cout << "\n";

    ListUtil::eraseCircular(lst, bb, ee);

    for(auto c : lst)
        std::cout << c << " ";
    std::cout << "\n";


    MeshTriangulation mesh(pts);
    //mesh.triangulate();
    std::cout << mesh.wkt() << std::endl;

//    std::cout << std::endl;
//    TriangulationFlipGraph gr({{0, 0}, {1, 0}, {0, 1}});
//    gr.generateGraph();
//
//    for(const auto& m : gr.vertices())
//        std::cout << m.wkt() << std::endl;
//    for(const auto& [h, t] : gr.edges())
//        std::cout << "{" << h << "," << t << "} ";
//    std::cout << std::endl;

    return 0;
}
