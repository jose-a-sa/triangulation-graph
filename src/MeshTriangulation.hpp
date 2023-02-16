#ifndef CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
#define CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP

#include <vector>
#include <set>
#include <initializer_list>
#include <unordered_map>
#include <string>

#include "MeshCells.hpp"
#include "Point.hpp"

class MeshTriangulation
{
public:
	MeshTriangulation(const MeshTriangulation& mesh);

	explicit MeshTriangulation(const std::vector<Point<int, 2>>& pt);

	explicit MeshTriangulation(std::vector<Point<int, 2>>&& pt);

	MeshTriangulation(std::initializer_list<Point<int, 2>>&& lst);

	~MeshTriangulation();

	void triangulateDelaunay();

	void flipEdge(const LineCell& l);

	const std::set<LineCell>& lines() const;

	const std::set<TriangleCell>& triangles() const;

	const std::unordered_map<LineCell, LineCell>& flippableLines() const;

	const std::unordered_map<LineCell, std::set<TriangleCell>>& edgeTriangleAdjacency() const;

	bool operator==(const MeshTriangulation& other) const;

	std::string wkt() const;

	friend std::size_t hash_value(const MeshTriangulation& mesh);

private:
	std::vector<Point<int, 2>> points_;
	std::set<LineCell> lines_;
	std::set<TriangleCell> triangles_;
	std::unordered_map<LineCell, std::set<TriangleCell>> edgeTrigAdj_;
	std::unordered_map<LineCell, LineCell> flippable_;

	void computeConnectivity_();

	void computeConnectivity_(const LineCell& l);

	double triangleArea_(std::size_t a, std::size_t b, std::size_t c) const;

	bool convexPolygon_(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const;
};

template<>
struct std::hash<MeshTriangulation>
{
	std::size_t operator()(const MeshTriangulation& t) const
	{
		return boost::hash<MeshTriangulation>()(t);
	}
};

#endif //CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
