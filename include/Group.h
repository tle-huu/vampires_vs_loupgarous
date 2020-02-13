#ifndef GROUP_H
#define GROUP_H

#include <stdint.h> 	/* (u)intN_t */

#include "Point.h"

/**
 * Represents a group of units in the map
 */
class Group
{
public:

    Group(Point position, uint16_t number) : m_position(position), m_number(number) {}

    Point position() const { return m_position; }
    uint16_t number() const { return m_number; }


protected:

    Point m_position;
    uint16_t m_number;
};

/**
 * Calculate distance between two groups in a map
 * @param group1 The first group
 * @param group2 The second group
 * @return The distance between these two groups
 */
uint16_t distance(Group const& group1, Group const& group2);

/**
 * Represents a group of player's units in the map
 */
class Gentil : public Group
{
public:

    Gentil(Point position, uint16_t number) : Group(position, number) {}
};

/**
 * Represents a group of opponent's units in the map
 */
class Vilain : public Group
{
public:

    Vilain(Point position, uint16_t number) : Group(position, number) {}
};

/**
 * Represents a group of human units in the map
 */
class Human : public Group
{
public:

    Human(Point position, uint16_t number) : Group(position, number) {}
};

#endif // GROUP_H
