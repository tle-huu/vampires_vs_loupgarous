#ifndef Point_h_INCLUDED
#define Point_h_INCLUDED

#include <stdint.h> 	/* int16_t */
#include <string>

/**
 * Represents a 2D point in the game map
 */
class Point
{
public:

    Point(int16_t x = 0, int16_t y = 0) : m_x(x), m_y(y) {}

    int16_t x() const { return m_x; }
    int16_t y() const { return m_y; }

    std::string to_string() const;


private:

    int16_t m_x;
    int16_t m_y;
};

bool operator==(Point const& point1, Point const& point2);
bool operator!=(Point const& point1, Point const& point2);

/**
 * Calculate distance between two points in a map
 * @param point1 The first point
 * @param point2 The second point
 * @return The distance between these two points
 */
int16_t distance(Point const& point1, Point const& point2);

#endif // Point_h_INCLUDED
