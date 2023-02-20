// unittest_SimpleMath.cpp

#include <gtest/gtest.h>
#include "../src/Point.hpp"


TEST(PointTest, at_inBounds)
{
    Point pt(23, -17);
    EXPECT_EQ(pt.at(0), 23);
    EXPECT_EQ(pt.at(1), -17);
}

TEST(PointTest, at_outOfBounds)
{
    Point pt;
    try
    {
        pt.at(2);
        FAIL() << "Expected std::out_of_range";
    }
    catch(std::out_of_range const& err)
    {
        EXPECT_EQ(err.what(), std::string("Point has only 2 elements"));
    }
}

TEST(PointTest, initializerList_inBounds)
{
    Point pt1{3, 5};
    EXPECT_EQ(pt1.x, 3);
    EXPECT_EQ(pt1.y, 5);

    Point pt2({7, 11});
    EXPECT_EQ(pt2.x, 7);
    EXPECT_EQ(pt2.y, 11);
}

TEST(PointTest, initializerList_outOfBounds)
{
    try
    {
        Point pt{0, 1, 2};
        FAIL() << "Expected std::out_of_range";
    }
    catch(std::out_of_range const& err)
    {
        EXPECT_EQ(err.what(), std::string("Initializer list must be dimension 2"));
    }

    try
    {
        Point pt{0};
        FAIL() << "Expected std::out_of_range";
    }
    catch(std::out_of_range const& err)
    {
        EXPECT_EQ(err.what(), std::string("Initializer list must be dimension 2"));
    }
}

TEST(PointTest, toString_0_m17)
{
    Point pt(0, -17);
    EXPECT_EQ(pt.toString(), "{0,-17}");
}

TEST(PointTest, toString_m127_12)
{
    Point pt(-127, 12);
    EXPECT_EQ(pt.toString(), "{-127,12}");
}

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