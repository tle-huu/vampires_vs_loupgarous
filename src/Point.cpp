#include "Point.h"

#include "utils.h"

std::string Point::to_string() const
{
	return int_to_string(m_x) + "x" + int_to_string(m_y);
}

bool operator==(Point const& point1, Point const& point2)
{
	return (point1.x() == point2.x()) && (point1.y() == point2.y());
}

int16_t distance(Point const& point1, Point const& point2)
{
    return max(abs(point1.x() - point2.x()), abs(point1.y() - point2.y()));
}
