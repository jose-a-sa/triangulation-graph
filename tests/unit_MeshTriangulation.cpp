// unittest_SimpleMath.cpp

#include <gtest/gtest.h>
#include "Factory.hpp"
#include "MeshTriangulation.hpp"

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
    auto mesh = MeshFactory::makeMesh(
        {Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1)});
    auto [l, lf] = *mesh->flippableLines().begin();
    mesh->flipLine(l);
    auto [l_f, lf_f] = *mesh->flippableLines().begin();
    EXPECT_EQ(l, lf_f);
    EXPECT_EQ(l_f, lf);
}

class MeshTriangulationFixture : public ::testing::Test
{
protected:
    MeshTriangulation m_Mesh;
};


TEST_F(MeshTriangulationFixture, pointCopyConstructor)
{
    auto pts = PointFactory::randomSample(3, 0, 1);
    m_Mesh = MeshTriangulation(pts);
    std::sort(pts.begin(), pts.end());
    EXPECT_EQ(m_Mesh.coordinates(), pts);

    pts = {{0.5, 0.1}, {0.5, 0.1}, {0.5, 0.1}, {-0.1, 1.0}, {-0.1, 1.0}};
    std::vector<Point> expectedSet({{-0.1, 1.0}, {0.5, 0.1}});
    m_Mesh = MeshTriangulation(pts);
    EXPECT_EQ(m_Mesh.coordinates(), expectedSet);
}

TEST_F(MeshTriangulationFixture, pointMoveConstructor)
{
    auto pts = PointFactory::randomSample(3, 0, 1);
    auto pts_c = pts;
    m_Mesh = MeshTriangulation(std::move(pts_c));
    std::sort(pts.begin(), pts.end());
    EXPECT_TRUE(m_Mesh.coordinates() == pts);

    pts = {{0.5, 0.1}, {0.5, 0.1}, {0.5, 0.1}, {-0.1, 1.0}, {-0.1, 1.0}};
    std::vector<Point> expectedSet({{-0.1, 1.0}, {0.5, 0.1}});
    m_Mesh = MeshTriangulation(std::move(pts));
    EXPECT_EQ(m_Mesh.coordinates(), expectedSet);
}