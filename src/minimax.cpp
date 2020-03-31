#include "minimax.h"

#include <iostream>
#include <vector>
#include <math.h>		/* round */

#include "utils.h"

Action get_best_action(Map *map, int16_t max_depth)
{
    // Create initial state
    State origin(map);

    // Get action
    return expect_minimax_alphabeta(&origin, max_depth);
}

Action minimax(State *origin, int16_t max_depth)
{
    Action best_action;
    int16_t best_value = INT16_MIN;
    for (Action const& action : origin->actions())
    {
        State child = origin->result(action);
        int16_t value = min_value(&child, max_depth - 1);
        if (value > best_value)
        {
            best_action = action;
            best_value = value;
        }
    }
    return best_action;
}

int16_t max_value(State *state, int16_t depth)
{
    if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
    int16_t value = INT16_MIN;
    for (State &child : state->successors())
    {
        value = max(value, min_value(&child, depth - 1));
    }
    return value;
}

int16_t min_value(State *state, int16_t depth)
{
    if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
    int16_t value = INT16_MAX;
    for (State &child : state->successors())
    {
        value = min(value, max_value(&child, depth - 1));
    }
    return value;
}

Action minimax_alphabeta(State *origin, int16_t max_depth)
{
    Action best_action;
    int16_t best_value = INT16_MIN;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
    for (Action const& action : origin->actions())
    {
        State child = origin->result(action);
        alpha = max(alpha, min_value_alphabeta(&child, max_depth - 1, alpha, beta));
        if (alpha > best_value)
        {
            best_action = action;
            best_value = alpha;
        }
    }
    return best_action;
}

int16_t max_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta)
{
	if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
    for (State &child : state->successors())
    {
        alpha = max(alpha, min_value_alphabeta(&child, depth - 1, alpha, beta));
        if (alpha >= beta)
        {
        	return alpha;
        }
    }
    return alpha;
}

int16_t min_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta)
{
	if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
    for (State &child : state->successors())
    {
        beta = min(beta, max_value_alphabeta(&child, depth - 1, alpha, beta));
        if (alpha >= beta)
        {
        	return beta;
        }
    }
    return beta;
}

Action expect_minimax_alphabeta(State *origin, int16_t max_depth)
{
    Action best_action;
    int16_t best_value = INT16_MIN;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
    for (Action const& action : origin->actions())
    {
        State child = origin->result(action);
        alpha = max(alpha, expect_minimax_alphabeta(&child, max_depth - 1, alpha, beta));
        if (alpha > best_value)
        {
            best_action = action;
            best_value = alpha;
        }
    }
    return best_action;
}

int16_t expect_minimax_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta)
{
	// If it's a chance node
	if (state->is_chance())
	{
		double value = 0;
		for (State &child : state->successors())
	    {
	        value += child.proba() * (double) expect_minimax_alphabeta(&child, depth, alpha, beta);
	    }
	    return (int16_t) round(value);
	}
	else
	{
		if (depth <= 0 || state->is_terminal())
	    {
	        return state->utility();
	    }

	    // If it's a Max node
	    if (state->is_max())
	    {
	    	for (State &child : state->successors())
		    {
		        alpha = max(alpha, expect_minimax_alphabeta(&child, depth - 1, alpha, beta));
		        if (alpha >= beta)
		        {
		        	return alpha;
		        }
		    }
		    return alpha;
	    }

	    // If it's a Min node
	    else
	    {
	    	for (State &child : state->successors())
		    {
		        beta = min(beta, expect_minimax_alphabeta(&child, depth - 1, alpha, beta));
		        if (alpha >= beta)
		        {
		        	return beta;
		        }
		    }
		    return beta;
	    }
	}
}
