#include <iostream>
#include <chrono>
#include <random>

#include <boost/polygon/voronoi.hpp>

#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"

namespace boost::polygon
{
	template<>
	struct geometry_concept<Point>
	{
		typedef point_concept type;
	};

	template<>
	struct point_traits<Point>
	{
		typedef double coordinate_type;

		static inline coordinate_type
		get(const Point& point, const orientation_2d& orient)
		{
			return (orient == HORIZONTAL) ? point.x : point.y;
		}
	};
}

class Timer
{
public:
	Timer(bool printOnInvalidation = true)
	{
		printOnInvalidation_ = printOnInvalidation;
		startTimepoint_ = std::chrono::high_resolution_clock::now();
	}
	~Timer()
	{
		if(printOnInvalidation_)
			std::cout << "Timer :" << Duration() << "us";
	}
	double Duration()
	{
		auto endTimepoint_ = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint_).time_since_epoch();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint_).time_since_epoch();
		std::chrono::duration<double, std::micro> duration = end - start;
		return duration.count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint_;
	bool printOnInvalidation_;
};

class RandomPoint
{
public:
	RandomPoint(double a = 0.0L, double b = 1.0L)
	{
		std::random_device rd;
		eng_ = std::mt19937(rd());
		dist_ = std::uniform_real_distribution<>(a, b);
	}
	~RandomPoint() = default;
	std::vector<Point> generate(std::size_t n = 1)
	{
		std::vector<Point> res;
		for(std::size_t i = 0; i < n; i++)
		{
			Point pt(dist_(eng_), dist_(eng_));
			res.push_back(std::move(pt));
		}
		return res;
	}
private:
	std::mt19937 eng_;
	std::uniform_real_distribution<> dist_;
};

int main(int argc, const char* argv[])
{
	const std::size_t sampleSize = 20;
	RandomPoint randomPoint(-1.0, 1.0);
	std::vector<Point> pts = randomPoint.generate(7);

	for(const auto& pt : pts)
		std::cout << pt.toString() << " ";
	std::cout << "\n\n";

	boost::polygon::voronoi_diagram<double> vd;
	boost::polygon::construct_voronoi(pts.begin(), pts.end(), &vd);

	std::cout << vd.vertices().size() << " " << vd.edges().size() << "\n";

	for(auto eg : vd.edges())
	{
		if(eg.vertex0() != nullptr)
			std::cout << "{" << eg.vertex0()->x() << "," << eg.vertex0()->y() << "}" << ' ';
		if(eg.vertex1() != nullptr)
			std::cout << "{" << eg.vertex1()->x() << "," << eg.vertex1()->y() << "}";
		std::cout << "\n";
	}


	MeshTriangulation mesh(pts);
	mesh.triangulateDelaunay();
	std::cout << mesh.wkt() << std::endl;
//	for(std::size_t n = 4; n < 14; n++)
//	{
//		std::vector<double> sampleTimers;
//		for(size_t k = 0; k < sampleSize; k++)
//		{
//			std::vector<Point> pts = randomPoint.generate(n);
//			TriangulationFlipGraph gr(std::move(pts));
//			Timer timeit(false);
//			gr.generateGraph();
//			sampleTimers.push_back(timeit.Duration());
//			if(k==0)
//				for(const auto& mesh : gr.vertices())
//					std::cout << mesh.wkt() << "\n";
//		}
//		auto mean = std::reduce(sampleTimers.begin(), sampleTimers.end()) / static_cast<double>(sampleSize);
//		std::cout << "Timer mean: " << mean << "us\n";
//	}

	std::cout << std::endl;
	TriangulationFlipGraph gr(pts);
	gr.generateGraph();

	for(const auto& mesh : gr.vertices())
		std::cout << mesh.wkt() << std::endl;
	for(const auto& [h, t] : gr.edges())
		std::cout << "{" << h << "," << t << "} ";
	std::cout << std::endl;

	return 0;
}
