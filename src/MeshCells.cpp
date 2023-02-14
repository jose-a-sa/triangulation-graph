#include "MeshCells.hpp"

#include <boost/assert.hpp>
#include <set>
#include <utility>
#include <iostream>


LineCell::LineCell() = default;

LineCell::LineCell(std::size_t a, std::size_t b)
	: a(a < b ? a : b), b(a < b ? b : a)
{
}

LineCell::LineCell(const LineCell& l) = default;

LineCell::LineCell(const std::set<std::size_t>& st)
{
	BOOST_ASSERT_MSG(st.size() >= 2,
		"std::set<std::size_t> initializer is not size 2, in LineCell::LineCell");
	auto it = st.begin();
	a = *(it++);
	b = *(it++);
}

LineCell::~LineCell() = default;

bool LineCell::operator<(const LineCell& rhs) const
{
	return (this->a != rhs.a) ? this->a < rhs.a : this->b < rhs.b;
}

bool LineCell::operator==(const LineCell& rhs) const
{
	return this->a == rhs.a && this->b == rhs.b;
}

bool LineCell::operator!=(const LineCell& rhs) const
{
	return !this->operator==(rhs);
}

TriangleCell::TriangleCell() = default;

TriangleCell::TriangleCell(std::size_t a, std::size_t b, std::size_t c)
{
	if (a > b)
		std::swap(a, b);
	if (b > c)
		std::swap(b, c);
	if (a > b)
		std::swap(a, b);

	this->a = a, this->b = b, this->c = c;
}

TriangleCell::TriangleCell(const TriangleCell& t) = default;

TriangleCell::TriangleCell(const std::set<std::size_t>& st)
{
	BOOST_ASSERT_MSG(st.size() >= 3,
		"std::set<std::size_t> initializer is not size 2, in LineCell::LineCell");
	auto it = st.begin();
	a = *(it++);
	b = *(it++);
	c = *(it++);
}

TriangleCell::~TriangleCell() = default;

bool TriangleCell::operator<(const TriangleCell& rhs) const
{
	if (this->a != rhs.a)
		return this->a < rhs.a;
	if (this->b != rhs.b)
		return this->b < rhs.b;

	return this->c < rhs.c;
}

bool TriangleCell::operator==(const TriangleCell& rhs) const
{
	return this->a == rhs.a && this->b == rhs.b && this->c == rhs.c;
}

bool TriangleCell::operator!=(const TriangleCell& rhs) const
{
	return !this->operator==(rhs);
}

std::set<std::size_t> TriangleCell::difference(const LineCell& l) const
{
	std::set<std::size_t> res;
	if (this->a != l.a && this->a != l.b)
		res.insert(this->a);
	if (this->b != l.a && this->b != l.b)
		res.insert(this->b);
	if (this->c != l.a && this->c != l.b)
		res.insert(this->c);
	return res;
}

bool TriangleCell::contains(const LineCell& l) const
{
	if (this->a == l.a)
		return this->b == l.b || this->c == l.b;
	return this->b == l.a && this->c == l.b;
}

std::ostream& operator<<(std::ostream& out, const LineCell& l)
{
	out << "{" << l.a << "," << l.b << "}";
	return out;
}

std::ostream& operator<<(std::ostream& out, const TriangleCell& t)
{
	out << "{" << t.a << "," << t.b << "," << t.c << "}";
	return out;
}

std::size_t hash_value(const LineCell& l)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, l.a);
	boost::hash_combine(seed, l.b);
	return seed;
}

std::size_t hash_value(const TriangleCell& t)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, t.a);
	boost::hash_combine(seed, t.b);
	boost::hash_combine(seed, t.c);
	return seed;
}
