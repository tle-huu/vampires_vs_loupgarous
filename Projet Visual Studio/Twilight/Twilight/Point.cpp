#include "Point.h"

#include "utils.h"

const Point Point::NULL_POINT = Point(-1, -1);

Point& Point::operator=(Point const& point)
{
	if (this != &point)
	{
		m_x = point.m_x;
		m_y = point.m_y;
	}
	return *this;
}

std::string Point::to_string() const
{
	return "(" + int_to_string(m_x) + "," + int_to_string(m_y) + ")";
}

bool operator==(Point const& point1, Point const& point2)
{
	return (point1.x() == point2.x()) && (point1.y() == point2.y());
}

bool operator!=(Point const& point1, Point const& point2)
{
	return !(point1 == point2);
}

std::ostream& operator<<(std::ostream &out, Point const& point)
{
	out << point.to_string();
	return out;
}

int16_t distance(Point const& point1, Point const& point2)
{
    return max(abs(point1.x() - point2.x()), abs(point1.y() - point2.y()));
}

Point next_point(Point const& src, Point const& dst, Point const& avoid)
{
	int16_t dx = sign(dst.x() - src.x());
	int16_t dy = sign(dst.y() - src.y());
	Point next(src.x() + dx, src.y() + dy);
	if (next == avoid)
	{
		if (dx == 0)
		{
			dx = src.x() > 0 ? -1 : 1;
		}
		else if (dy == 0)
		{
			dy = src.y() > 0 ? -1 : 1;
		}
		else if (abs(dst.x() - src.x()) < abs(dst.y() - src.y()))
		{
			dx = 0;
		}
		else
		{
			dy = 0;
		}
		next = Point(src.x() + dx, src.y() + dy);
	}
	return next;
}
