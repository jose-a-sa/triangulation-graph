#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stack>
#include <vector>

#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"

template<typename T>
void parseArgument(std::vector<Point<T, 2>>& res, const std::string& sv)
{
	auto numParser = [&](const std::string& s) -> T
	{
		T res;
		try
		{
			res = boost::lexical_cast<T>(s);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			std::cerr << e.what() << '\n';
			std::exit(1);
		}
		return res;
	};

	auto stackToPointVector = [&numParser, &res](std::stack<std::string>& st) -> void
	{
		if (st.size() != 2)
		{
			std::cerr << "ERROR: Dimension of point is not 2." << std::endl;
			std::exit(1);
		}
		Point<T, 2> pt;
		pt[0] = numParser(st.top());
		st.pop();
		pt[1] = numParser(st.top());
		st.pop();
		res.push_back(pt);
	};

	std::string curr;
	std::stack<std::string> st;

	for (const char& c : sv)
	{
		if (c == '(' || isspace(c))
			continue;
		else if (c == ')' || c == ',')
		{
			st.push(curr);
			if (c == ')')
				stackToPointVector(st);
			curr.clear();
		}
		else
			curr.push_back(c);
	}
}

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "ERROR: Expecting 1 argument as a string of points." << std::endl;
		std::exit(1);
	}

	std::string arg_coords(argv[1]);
	std::vector<Point<double, 2>> coords;
	parseArgument<double>(coords, arg_coords);

	TriangulationFlipGraph gr(std::move(coords));
	gr.generateGraph();

	for (const auto& mesh : gr.vertices())
		std::cout << mesh.wkt() << std::endl;
	for (const auto& eg : gr.edges())
		std::cout << eg << " ";
	std::cout << std::endl;

	return 0;
}