#include <iostream>

#include "Point.hpp"

int main(int argc, char const *argv[])
{
    Point<int, 2> pt{1,2};
    std::cout << pt << '\n';   
    return 0;
}
