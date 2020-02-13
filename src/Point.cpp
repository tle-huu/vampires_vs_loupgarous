#include "../include/Point.h"

#include <stdint.h> 	/* (u)intN_t */
#include <string>
#include <algorithm>	/* std::max */

#include "../include/utils.h"

std::string Point::to_string() const
{
	return int_to_string(m_x) + "x" + int_to_string(m_y);
}

uint16_t distance(Point const& point1, Point const& point2)
{
    return std::max(abs(point1.x() - point2.x()), abs(point1.y() - point2.y()));
}
