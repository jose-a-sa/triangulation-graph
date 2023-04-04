#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stack>
#include <vector>

#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"

void parseArgument(std::vector<Point>& res, const std::string& sv)
{
    auto numParser = [&](const std::string& s) -> std::double_t
    {
        std::double_t res;
        try
        {
            res = boost::lexical_cast<std::double_t>(s);
        }
        catch(const boost::bad_lexical_cast& e)
        {
            std::cerr << e.what() << '\n';
            std::exit(1);
        }
        return res;
    };

    auto stackToPointVector = [&](std::stack<std::string>& st) -> void
    {
        BOOST_ASSERT_MSG(st.size() == 2,
                         "Point has only 2 elements, in parseArgument");

        Point pt;
        pt.x = numParser(st.top());
        st.pop();
        pt.y = numParser(st.top());
        st.pop();
        res.push_back(pt);
    };

    std::string curr;
    std::stack<std::string> st;

    for(const char& c : sv)
    {
        if(c == '(' || isspace(c))
            continue;
        else if(c == ')' || c == ',')
        {
            st.push(curr);
            if(c == ')')
                stackToPointVector(st);
            curr.clear();
        }
        else
            curr.push_back(c);
    }
}

int main(int argc, const char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "ERROR: Expecting 1 argument as a string of points." << std::endl;
        std::exit(1);
    }

    std::string arg_coords(argv[1]);
    std::vector<Point> coords;
    parseArgument(coords, arg_coords);

    TriangulationFlipGraph gr(coords);
    gr.generateGraph();

    for(const auto& mesh : gr.vertices())
        std::cout << mesh.toString() << std::endl;
    for(const auto& [h, t] : gr.edges())
        std::cout << "{" << h << "," << t << "} ";
    std::cout << std::endl;

    return 0;
}