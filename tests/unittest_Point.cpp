// unittest_SimpleMath.cpp

#include <gtest/gtest.h>
#include "../src/Point.hpp"


TEST(PointTest, Distance_3_4)
{
    Point oo;
    Point pt(3, 4);
    EXPECT_NEAR(oo.distance(pt), 5, 0.0000000001);
}

TEST(PointTest, Distance_7_11)
{
    Point oo;
    Point pt(7, 11);
    EXPECT_NEAR(oo.distance(pt), 13.0384048104, 0.0000000001);
}