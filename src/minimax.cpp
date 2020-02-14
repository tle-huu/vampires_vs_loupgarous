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

Action minimax_alphabeta(State &state)
{
	State *best_state = 0;
    int16_t best_value = INT16_MIN;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
    for (State *s : state.successors())
    {
        alpha = std::max(alpha, min_value_alphabeta(s, alpha, beta));
        if (alpha > best_value)
        {
            best_state = s;
            best_value = alpha;
        }
    }
    return state.action(best_state);
}

int16_t max_value_alphabeta(State *state, int16_t alpha, int16_t beta)
{
	if (state->is_terminal())
    {
        return state->utility();
    }
    for (State *s : state->successors())
    {
        alpha = std::max(alpha, min_value_alphabeta(s, alpha, beta));
        if (alpha >= beta)
        {
        	return alpha;
        }
    }
    return alpha;
}

int16_t min_value_alphabeta(State *state, int16_t alpha, int16_t beta)
{
	if (state->is_terminal())
    {
        return state->utility();
    }
    for (State *s : state->successors())
    {
        beta = std::min(beta, max_value_alphabeta(s, alpha, beta));
        if (alpha >= beta)
        {
        	return beta;
        }
    }
    return beta;
}
