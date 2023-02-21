#include <iostream>

#include "Utils.hpp"
#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"


int main(int argc, const char* argv[])
{
    for(size_t k = 1; k < 100; k++)
    {
        std::size_t sampleSize = 10;
        std::vector<double> dur(sampleSize);
        for(std::size_t i = 0; i < sampleSize; i++)
        {
            std::vector<Point> pts = PointFactory::randomSample(10 * k);
            MeshTriangulation mesh(pts);
            Timer timeit(false);
            mesh.triangulate();
            dur[i] = timeit.Duration();
        }

        std::cout << "Mean duration: " << (std::accumulate(dur.begin(), dur.end(), 0.0) / dur.size()) << "us\n";
    }

    return 0;
}
