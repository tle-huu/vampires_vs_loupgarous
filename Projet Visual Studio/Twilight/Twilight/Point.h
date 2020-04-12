#ifndef Point_h_INCLUDED
#define Point_h_INCLUDED

#include <stdint.h> 	/* int16_t */
#include <string>
#include <iostream>

/**
 * Represents a 2D point in the game map
 */
class Point
{
public:

	static const Point NULL_POINT;

    Point(int16_t x = -1, int16_t y = -1) : m_x(x), m_y(y) {}

	Point& operator=(Point const& point);

    int16_t x() const { return m_x; }
    int16_t y() const { return m_y; }

    std::string to_string() const;


private:

    int16_t m_x;
    int16_t m_y;
};

bool operator==(Point const& point1, Point const& point2);
bool operator!=(Point const& point1, Point const& point2);

std::ostream& operator<<(std::ostream &out, Point const& point);

/**
 * Calculate distance between two points in a map
 * @param point1 The first point
 * @param point2 The second point
 * @return The distance between these two points
 */
int16_t distance(Point const& point1, Point const& point2);

/**
 * Find a next point to move to go from a source to a destination
 * @param src The source point
 * @param dst The destination point
 * @param avoid An optional point to avoid
 * @return The next point to move
 */
Point next_point(Point const& src, Point const& dst, Point const& avoid = Point::NULL_POINT);

#endif // Point_h_INCLUDED
