#ifndef POINT_H
#define POINT_H

#include <stdint.h> 	/* (u)intN_t */
#include <string>

/**
 * Represents a 2D point in the game map
 */
class Point
{
public:

    Point(uint8_t x = 0, uint8_t y = 0) : m_x(x), m_y(y) {}

    uint8_t x() const { return m_x; }
    uint8_t y() const { return m_y; }

    std::string to_string() const;


private:

    const uint8_t m_x;
    const uint8_t m_y;
};

/**
 * Calculate distance between two points in a map
 * @param point1 The first point
 * @param point2 The second point
 * @return The distance between these two points
 */
uint16_t distance(Point const& point1, Point const& point2);

#endif // POINT_H
