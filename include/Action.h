#ifndef ACTION_H
#define ACTION_H

#include <stdint.h>     /* (u)intN_t */
#include <string>
#include <vector>

#include "Point.h"

/**
 * Represents a move of a group in the map
 */
class Move
{
public:

    Move(uint8_t x1, uint8_t y1, uint16_t n, uint8_t x2, uint8_t y2) : m_start(x1, y1), m_end(x2, y2), m_number(n) {}

    std::string to_string() const;


private:

    Point m_start;
    Point m_end;
    uint16_t m_number;
};

/**
 * Represents moves of a game turn
 */
class Action
{
public:

    Action() {}
    Action(Move move) { add_move(move); }

    void add_move(Move move) { m_moves.push_back(move); }
    int moves_number() const { return m_moves.size(); }

    std::string to_string() const;
    char* to_cstring() const;


private:

    std::vector<Move> m_moves;
};

#endif // ACTION_H
