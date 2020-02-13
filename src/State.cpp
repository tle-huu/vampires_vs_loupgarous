#include "../include/State.h"

#include <stdint.h>     /* (u)intN_t */
#include <vector>

#include "../include/Group.h"
#include "../include/Map.h"
#include "../include/Action.h"

int16_t State::utility()
{
    if (m_utility == INT16_MIN)
    {
        m_utility = calculate_utility(m_map);
    }
    return m_utility;
}

Action State::action(State *state) const
{
    Action action;
    if (state != 0)
    {
        //TODO
    }
    return action;
}

int16_t heuristic(Map const& map)
{
    int16_t value = 0;
    //TODO
    return value;
}

int16_t calculate_utility(Map const& map)
{
    int16_t value = 0;
    for (Gentil const& gentil : map.gentils())
    {
        value += gentil.number();
    }
    return value + heuristic(map);
}
