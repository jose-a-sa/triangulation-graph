// unittest_SimpleMath.cpp

#include <gtest/gtest.h>
#include "../src/MeshTriangulation.hpp"
#include "../src/Utils.hpp"

TEST(MeshTriangulationTest, Single_is1Triangle)
{
    auto mesh = MeshFactory::makeMeshSingle(1);
    EXPECT_EQ(mesh->lines().size(), 3);
    EXPECT_EQ(mesh->triangles().size(), 1);
    EXPECT_EQ(mesh->flippableLines().size(), 0);
}

TEST(MeshTriangulationTest, Single_Small_is1Triangle)
{
    auto mesh = MeshFactory::makeMeshSingle(0.001);
    EXPECT_EQ(mesh->lines().size(), 3);
    EXPECT_EQ(mesh->triangles().size(), 1);
    EXPECT_EQ(mesh->flippableLines().size(), 0);
}

TEST(MeshTriangulationTest, Single_Large_is1Triangle)
{
    auto mesh = MeshFactory::makeMeshSingle(1000.);
    EXPECT_EQ(mesh->lines().size(), 3);
    EXPECT_EQ(mesh->triangles().size(), 1);
    EXPECT_EQ(mesh->flippableLines().size(), 0);
}

TEST(MeshTriangulationTest, Square_is2Triangles)
{
    auto mesh = MeshFactory::makeMesh({
                                          Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)});
    EXPECT_EQ(mesh->lines().size(), 5);
    EXPECT_EQ(mesh->triangles().size(), 2);
    EXPECT_EQ(mesh->flippableLines().size(), 1);
}

TEST(MeshTriangulationTest, Square_flipped_is2Triangles)
{
    auto mesh = MeshFactory::makeMesh({
                                          Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)});
    auto [l, lf] = *mesh->flippableLines().begin();
    mesh->flipLine(l);
    EXPECT_EQ(mesh->lines().size(), 5);
    EXPECT_EQ(mesh->triangles().size(), 2);
    EXPECT_EQ(mesh->flippableLines().size(), 1);
}

TEST(MeshTriangulationTest, Square_flippedTwice_isIdentity)
{
    auto mesh = MeshFactory::makeMesh({
                                          Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)
                                      });
    auto [l, lf] = *mesh->flippableLines().begin();
    mesh->flipLine(l);
    auto [l_f, lf_f] = *mesh->flippableLines().begin();
    EXPECT_EQ(l, lf_f);
    EXPECT_TRUE(l == lf_f);
    EXPECT_EQ(l_f, lf);
    EXPECT_TRUE(l_f == lf);
}
