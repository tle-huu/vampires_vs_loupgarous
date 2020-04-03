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
	State best_child;
    int16_t best_value = INT16_MIN;
    for (Action const& action : origin->actions())
    {
        State child = origin->result(action);
        int16_t value = min_value(&child, max_depth - 1);
        if (value > best_value || (value == best_value && child.preferable_than(&best_child)))
        {
            best_action = action;
			best_child = child;
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
	State best_child;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
    for (Action const& action : origin->actions())
    {
        State child = origin->result(action);
		int16_t value = min_value_alphabeta(&child, max_depth - 1, alpha, beta);
        if (value > alpha || (value == alpha && child.preferable_than(&best_child)))
        {
			best_action = action;
			best_child = child;
        }
		alpha = max(alpha, value);
    }
    return best_action;
}

int16_t max_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta)
{
	if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
	int16_t value = INT16_MIN;
    for (State &child : state->successors())
    {
		value = max(value, min_value_alphabeta(&child, depth - 1, alpha, beta));
		alpha = max(alpha, value);
		if (alpha >= beta)
        {
			break;
        }
    }
    return value;
}

int16_t min_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta)
{
	if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
	int16_t value = INT16_MAX;
    for (State &child : state->successors())
    {
        value = min(value, max_value_alphabeta(&child, depth - 1, alpha, beta));
		beta = min(beta, value);
        if (alpha >= beta)
        {
			break;
        }
    }
    return value;
}

Action expect_minimax_alphabeta(State *origin, int16_t max_depth)
{
    Action best_action;
	State best_child;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
	for (Action const& action : origin->actions())
	{
		State child = origin->result(action);
		int16_t value = expect_minimax_alphabeta(&child, max_depth - 1, alpha, beta);
		if (value > alpha || (value == alpha && child.preferable_than(&best_child)))
		{
			best_action = action;
			best_child = child;
std::cout << "best action = " << best_action << std::endl;
std::cout << "best alpha = " << value << std::endl;
		}
		alpha = max(alpha, value);
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
//std::cout << "expect utility = " << (int16_t) round(value) << std::endl;
	    return (int16_t) round(value);
	}
	else
	{
		// If it's a leaf node
		if (depth <= 0 || state->is_terminal())
	    {
	        return state->utility();
	    }

	    // If it's a Max node
	    if (state->is_max())
	    {
			int16_t value = INT16_MIN;
			for (State &child : state->successors())
		    {
		        value = max(value, expect_minimax_alphabeta(&child, depth - 1, alpha, beta));
				alpha = max(alpha, value);
		        if (alpha >= beta)
		        {
					break;
		        }
		    }
		    return value;
	    }

	    // If it's a Min node
	    else
	    {
			int16_t value = INT16_MAX;
			for (State &child : state->successors())
		    {
		        value = min(value, expect_minimax_alphabeta(&child, depth - 1, alpha, beta));
				beta = min(beta, value);
				if (alpha >= beta)
		        {
					break;
		        }
		    }
			return value;
	    }
	}
}
