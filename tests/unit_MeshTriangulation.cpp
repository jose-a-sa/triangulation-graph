// unittest_SimpleMath.cpp

#include <gtest/gtest.h>
#include "Factory.hpp"
#include "MeshTriangulation.hpp"

TEST(MeshTriangulationTest, Single_is1Triangle)
{
    auto pts = PointFactory::makeSimplex(1.);
    MeshTriangulation mesh(pts);
    mesh.triangulate();
    EXPECT_EQ(mesh.lines().size(), 3);
    EXPECT_EQ(mesh.triangles().size(), 1);
    EXPECT_EQ(mesh.flippableLines().size(), 0);
}

TEST(MeshTriangulationTest, Single_Small_is1Triangle)
{
    auto pts = PointFactory::makeSimplex(0.001);
    MeshTriangulation mesh(pts);
    mesh.triangulate();
    EXPECT_EQ(mesh.lines().size(), 3);
    EXPECT_EQ(mesh.triangles().size(), 1);
    EXPECT_EQ(mesh.flippableLines().size(), 0);
}

TEST(MeshTriangulationTest, Single_Large_is1Triangle)
{
    auto pts = PointFactory::makeSimplex(1000.);
    MeshTriangulation mesh(pts);
    mesh.triangulate();
    EXPECT_EQ(mesh.lines().size(), 3);
    EXPECT_EQ(mesh.triangles().size(), 1);
    EXPECT_EQ(mesh.flippableLines().size(), 0);
}

TEST(MeshTriangulationTest, Square_is2Triangles)
{
    std::vector<Point> pts({Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)});
    auto pts_ptr = std::make_shared<std::vector<Point>>(std::move(pts));
    MeshTriangulation mesh(pts_ptr);
    mesh.triangulate();
    EXPECT_EQ(mesh.lines().size(), 5);
    EXPECT_EQ(mesh.triangles().size(), 2);
    EXPECT_EQ(mesh.flippableLines().size(), 1);
}

TEST(MeshTriangulationTest, Square_flipped_is2Triangles)
{
    std::vector<Point> pts({Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)});
    auto pts_ptr = std::make_shared<std::vector<Point>>(std::move(pts));
    MeshTriangulation mesh(pts_ptr);
    mesh.triangulate();
    auto [l, lf] = *mesh.flippableLines().begin();
    mesh.flipLine(l);
    EXPECT_EQ(mesh.lines().size(), 5);
    EXPECT_EQ(mesh.triangles().size(), 2);
    EXPECT_EQ(mesh.flippableLines().size(), 1);
}

TEST(MeshTriangulationTest, Square_flippedTwice_isIdentity)
{
    std::vector<Point> pts({Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)});
    auto pts_ptr = std::make_shared<std::vector<Point>>(std::move(pts));
    MeshTriangulation mesh(pts_ptr);
    mesh.triangulate();
    auto [l, lf] = *mesh.flippableLines().begin();
    mesh.flipLine(l);
    auto [l_f, lf_f] = *mesh.flippableLines().begin();
    EXPECT_EQ(l, lf_f);
    EXPECT_EQ(l_f, lf);
}