#include "MeshTriangulation.hpp"

#include <boost/assert.hpp>
#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/polygon/voronoi.hpp>
#include <sstream>

namespace boost::polygon
{
	template<>
	struct geometry_concept<Point<double, 2>>
	{
		typedef point_concept type;
	};

	template<>
	struct point_traits<Point<double, 2>>
	{
		typedef double coordinate_type;

		static inline coordinate_type
		get(const Point<double, 2>& point, const orientation_2d& orient)
		{
			return (orient == HORIZONTAL) ? point[0] : point[1];
		}
	};
}

MeshTriangulation::MeshTriangulation(const MeshTriangulation& mesh) = default;

MeshTriangulation::MeshTriangulation(const std::vector<Point<double, 2>>& pt)
	: coord_(pt)
{
}

MeshTriangulation::MeshTriangulation(std::vector<Point<double, 2>>&& pt)
	: coord_(std::move(pt))
{
}

MeshTriangulation::MeshTriangulation(std::initializer_list<Point<double, 2>>&& lst)
	: coord_(lst)
{
}

MeshTriangulation::~MeshTriangulation() = default;

void MeshTriangulation::triangulateDelaunay()
{
	lines_.clear();
	triangles_.clear();

	boost::polygon::voronoi_diagram<double> vd;
	boost::polygon::construct_voronoi(coord_.begin(), coord_.end(), &vd);

	for (const auto& vertex : vd.vertices())
	{
		const boost::polygon::voronoi_edge<double>* base_edge = vertex.incident_edge();

		std::queue<const boost::polygon::voronoi_edge<double>*> bfs;
		bfs.push(base_edge->rot_next());
		std::size_t base_idx = base_edge->cell()->source_index();

		while (bfs.back() != base_edge && !bfs.empty())
		{
			const boost::polygon::voronoi_edge<double>* edge = bfs.back();
			std::size_t idx = edge->cell()->source_index();

			BOOST_ASSERT_MSG(edge->cell()->contains_point(),
				"LineCell cell does not contain point, in MeshTriangulation::triangulateDelaunay");

			lines_.insert(LineCell(base_idx, idx));

			if (bfs.size() == 2)
			{
				std::size_t next_idx = bfs.front()->cell()->source_index();
				lines_.insert(LineCell(idx, next_idx));
				triangles_.insert(TriangleCell(base_idx, idx, next_idx));
				bfs.pop();
			}

			bfs.push(edge->rot_next());
		}
	}

	computeConnectivity_();
}

void MeshTriangulation::flipEdge(const LineCell& l)
{
	if (!edgeTrigAdj_.count(l))
		return;

	if (!flippable_.count(l))
		return;

	const LineCell& l_flip = flippable_.at(l);

	lines_.insert(l_flip);
	TriangleCell t1_flip(l_flip.a, l_flip.b, l.a);
	TriangleCell t2_flip(l_flip.a, l_flip.b, l.b);
	triangles_.insert(t1_flip);
	triangles_.insert(t2_flip);

	std::set < TriangleCell > t_pair{t1_flip, t2_flip};
	edgeTrigAdj_[l_flip] = std::move(t_pair);

	std::vector<LineCell> bd_edges{
		LineCell(l.a, l_flip.a), LineCell(l.a, l_flip.b),
		LineCell(l_flip.a, l.b), LineCell(l.b, l_flip.b)};

	flippable_.insert({l_flip, l});

	for (const auto& t : edgeTrigAdj_.at(l))
		triangles_.erase(t);
	edgeTrigAdj_.erase(l);
	lines_.erase(l);
	flippable_.erase(l);

	for (const auto& e : bd_edges)
		computeConnectivity_(e);
}

inline double MeshTriangulation::triangleArea_(std::size_t a, std::size_t b, std::size_t c) const
{
	double s1, s2, s3;

	s1 = coord_[a].distance(coord_[b]);
	s2 = coord_[a].distance(coord_[c]);
	s3 = coord_[b].distance(coord_[c]);

	return sqrt((s1 + s2 + s3) * (-s1 + s2 + s3) * (s1 - s2 + s3) * (s1 + s2 - s3)) / 4;
}

inline bool MeshTriangulation::convexPolygon_(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const
{
	double t1, t2, t1_f, t2_f;
	t1 = triangleArea_(i, j, k), t2 = triangleArea_(k, l, i);
	t1_f = triangleArea_(j, k, l), t2_f = triangleArea_(l, i, j);

	return boost::math::epsilon_difference(t1, 0) > 3.0 &&
		boost::math::epsilon_difference(t2, 0) > 3.0 &&
		boost::math::epsilon_difference(t1_f, 0) > 3.0 &&
		boost::math::epsilon_difference(t2_f, 0) > 3.0 &&
		boost::math::epsilon_difference(t1 + t2, t1_f + t2_f) < 3.0;
}

const std::set<LineCell>& MeshTriangulation::lines() const
{
	return this->lines_;
}

const std::set<TriangleCell>& MeshTriangulation::triangles() const
{
	return this->triangles_;
}

const std::unordered_map<LineCell, LineCell>& MeshTriangulation::flippableLines() const
{
	return this->flippable_;
}

std::string MeshTriangulation::wkt() const
{
	std::ostringstream oss;
	oss << "MULTIPOLYGON(";
	for (const auto& t : triangles())
	{
		oss << "(";
		oss << coord_[t.a].wtk() << ",";
		oss << coord_[t.b].wtk() << ",";
		oss << coord_[t.c].wtk();
		oss << (t != *std::prev(triangles().end()) ? "), " : ")");
	}
	oss << ")";
	return oss.str();
}

const std::unordered_map<LineCell, std::set<TriangleCell>>& MeshTriangulation::edgeTriangleAdjacency() const
{
	return edgeTrigAdj_;
}

void MeshTriangulation::computeConnectivity_(const LineCell& l)
{
	if (edgeTrigAdj_.count(l))
		edgeTrigAdj_.at(l).clear();
	flippable_.erase(l);

	LineCell l_flip;
	for (const TriangleCell& t : triangles())
	{
		std::size_t o = t.otherPoint(l);
		if (o != -1)
		{
			edgeTrigAdj_[l].insert(t);
			l_flip.insert(o);
		}
	}

	if (l_flip.size() != 2)
		return;

	if (!convexPolygon_(l.a, l_flip.a, l.b, l_flip.b))
		return;

	flippable_.insert({l, l_flip});
}

void MeshTriangulation::computeConnectivity_()
{
	for (const LineCell& l : lines())
		computeConnectivity_(l);
}

std::size_t hash_value(const MeshTriangulation& mesh)
{
	std::size_t seed = 0;
	for (const auto& p : mesh.coordinates())
		boost::hash_combine(seed, boost::hash<Point<double,2>>()(p));
	for (const auto& t : mesh.triangles())
		boost::hash_combine(seed, boost::hash<TriangleCell>()(t));
	return seed;
}

bool MeshTriangulation::operator==(const MeshTriangulation& other) const
{
	return hash_value(*this) == hash_value(other);
}

const std::vector<Point<double, 2>>& MeshTriangulation::coordinates() const
{
	return coord_;
}
