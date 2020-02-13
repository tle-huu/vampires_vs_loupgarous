#ifndef MAP_H
#define MAP_H

#include <stdint.h>     /* (u)intN_t */
#include <vector>

#include "Group.h"

/**
 * Represents a map of the game
 */
class Map
{
public:

    Map(uint8_t lines, uint8_t columns) : m_lines(lines), m_columns(columns) {}

    uint8_t lines() const { return m_lines; }
    uint8_t columns() const { return m_columns; }
    std::vector<Gentil> gentils() const { return m_gentils; }
    std::vector<Vilain> vilains() const { return m_vilains; }
    std::vector<Human> humans() const { return m_humans; }

    void add_gentil(Gentil gentil) { m_gentils.push_back(gentil); }
    void add_vilain(Vilain vilain) { m_vilains.push_back(vilain); }
    void add_human(Human human) { m_humans.push_back(human); }


private:

    const uint8_t m_lines;
    const uint8_t m_columns;
    std::vector<Gentil> m_gentils;
    std::vector<Vilain> m_vilains;
    std::vector<Human> m_humans;
};

#endif // MAP_H
