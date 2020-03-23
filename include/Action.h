#ifndef ACTION_H
#define ACTION_H

#include <stdint.h>     /* int16_t */
#include <string>
#include <vector>

#include "Point.h"


/**
 * Represents a move of a group in the map
 */
class Move
{
public:

    Move(int16_t x1, int16_t y1, int16_t n, int16_t x2, int16_t y2) : m_start(x1, y1), m_end(x2, y2), m_number(n) {}
    Point start() const { return m_start; }
    Point end() const { return m_end; }
    int16_t number() const { return m_number; }

    std::string to_string() const;


private:

    Point m_start;
    Point m_end;
    int16_t m_number;
};

/**
 * Represents moves of a game turn
 */
class Action
{
public:

    Action();
    //Action(Action& act);
    Action(Move move);
    Action(std::string actionString);


    std::vector<Move> moves() const { return m_moves; }

    void add_move(Move move) { m_moves.push_back(move); }
    int moves_number() const { return m_moves.size(); }

    std::string to_string() const;
    char* to_cstring() const;


private:

    std::vector<Move> m_moves;
};

#endif // ACTION_H
