#ifndef STATE_H
#define STATE_H

#include <stdint.h>     /* (u)intN_t */
#include <vector>

#include "Map.h"
#include "Action.h"

/**
 * Represents a state of the game
 */
class State
{
public:

    State(Map map) : m_map(map), m_utility(INT16_MIN) {}

    Map map() const { return m_map; }
    std::vector<State*> successors() const { return m_successors; }

    int16_t utility();

    bool is_terminal() const { return m_successors.empty(); }

    Action action(State *state) const;


private:

    Map m_map;
    int16_t m_utility;
    std::vector<State*> m_successors;
};

/**
 * Calculate heuristic value of a map
 * TODO
 * @param map The map to calculate heuristic value of
 * @return The heuristic value of the map
 */
int16_t heuristic(Map const& map);

/**
 * Calculate utility of a map
 * Add all gentil units plus a heuristic value
 * @param map The map to calculate utility of
 * @return The utility of the map
 */
int16_t calculate_utility(Map const& map);

#endif // STATE_H
