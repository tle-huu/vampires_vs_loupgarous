#include "../include/minimax.h"

#include <stdint.h>     /* (u)intN_t */
#include <vector>
#include <algorithm>    /* std::min, std::max */

#include "../include/Action.h"
#include "../include/State.h"

void build_tree(State &initial_state, uint8_t depth)
{
    //TODO
}

Action minimax(State &state)
{
    State *best_state = 0;
    int16_t best_value = INT16_MIN;
    for (State *s : state.successors())
    {
        int16_t value = min_value(s);
        if (value > best_value)
        {
            best_state = s;
            best_value = value;
        }
    }
    return state.action(best_state);
}

int16_t max_value(State *state)
{
    if (state->is_terminal())
    {
        return state->utility();
    }
    int16_t value = INT16_MIN;
    for (State *s : state->successors())
    {
        value = std::max(value, min_value(s));
    }
    return value;
}

int16_t min_value(State *state)
{
    if (state->is_terminal())
    {
        return state->utility();
    }
    int16_t value = INT16_MAX;
    for (State *s : state->successors())
    {
        value = std::min(value, max_value(s));
    }
    return value;
}
