#include "minimax.h"

#include <vector>
#include <math.h>		/* round */

#include "utils.h"

Action get_best_action(Map *map, int16_t max_depth, int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
    // Initialize chrono
	Chrono chrono(MAX_TIME);
	
	// Create initial state
    State origin(map);

    // Get action
    return expect_minimax_alphabeta(&origin, max_depth, chrono, action_method, max_gentil_groups, max_vilain_groups);
}

Action minimax(State *origin, int16_t max_depth, int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
    Action best_action;
	State best_child;
    int16_t best_value = INT16_MIN;
    for (Action const& action : origin->actions(action_method, max_gentil_groups, max_vilain_groups))
    {
        State child = origin->result(action);
        int16_t value = min_value(&child, max_depth - 1, action_method, max_gentil_groups, max_vilain_groups);
        if (value > best_value || (value == best_value && child.preferable_than(&best_child)))
        {
            best_action = action;
			best_child = child;
            best_value = value;
        }
    }
    return best_action;
}

int16_t max_value(State *state, int16_t depth, int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
    if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
    int16_t value = INT16_MIN;
    for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
    {
        value = max(value, min_value(&child, depth - 1, action_method, max_gentil_groups, max_vilain_groups));
    }
    return value;
}

int16_t min_value(State *state, int16_t depth, int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
    if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
    int16_t value = INT16_MAX;
    for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
    {
        value = min(value, max_value(&child, depth - 1, action_method, max_gentil_groups, max_vilain_groups));
    }
    return value;
}

Action minimax_alphabeta(State *origin, int16_t max_depth, int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
    Action best_action;
	State best_child;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
    for (Action const& action : origin->actions(action_method, max_gentil_groups, max_vilain_groups))
    {
        State child = origin->result(action);
		int16_t value = min_value_alphabeta(&child, max_depth - 1, alpha, beta, action_method, max_gentil_groups, max_vilain_groups);
        if (value > alpha || (value == alpha && child.preferable_than(&best_child)))
        {
			best_action = action;
			best_child = child;
        }
		alpha = max(alpha, value);
    }
    return best_action;
}

int16_t max_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta, 
	int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
	if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
	int16_t value = INT16_MIN;
    for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
    {
		value = max(value, min_value_alphabeta(&child, depth - 1, alpha, beta, action_method, max_gentil_groups, max_vilain_groups));
		alpha = max(alpha, value);
		if (alpha >= beta)
        {
			break;
        }
    }
    return value;
}

int16_t min_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta, 
	int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
	if (depth <= 0 || state->is_terminal())
    {
        return state->utility();
    }
	int16_t value = INT16_MAX;
    for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
    {
        value = min(value, max_value_alphabeta(&child, depth - 1, alpha, beta, action_method, max_gentil_groups, max_vilain_groups));
		beta = min(beta, value);
        if (alpha >= beta)
        {
			break;
        }
    }
    return value;
}

Action expect_minimax_alphabeta(State *origin, int16_t max_depth, Chrono const& chrono, 
	int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
    Action best_action;
	State best_child;
    int16_t alpha = INT16_MIN;
    int16_t beta = INT16_MAX;
	for (Action const& action : origin->actions(action_method, max_gentil_groups, max_vilain_groups))
	{
		State child = origin->result(action);
		int16_t value = expect_minimax_alphabeta(&child, max_depth - 1, alpha, beta, chrono, action_method, max_gentil_groups, max_vilain_groups);
		if (value > alpha || (value == alpha && child.preferable_than(&best_child)))
		{
			best_action = action;
			best_child = child;
		}
		alpha = max(alpha, value);
	}
    return best_action;
}

int16_t expect_minimax_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta, Chrono const& chrono, 
	int action_method, int16_t max_gentil_groups, int16_t max_vilain_groups)
{
	// If it's a chance node
	if (state->is_chance())
	{
		double value = 0;
		for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
	    {
	        value += child.proba() * ((double) expect_minimax_alphabeta(&child, depth, alpha, beta, chrono, action_method, max_gentil_groups, max_vilain_groups));
	    }
	    return (int16_t) round(value);
	}
	else
	{
		// If it's a leaf node or if the calculation time is too long
		if (depth <= 0 || state->is_terminal() || chrono.alarm())
	    {
			return state->utility();
	    }

	    // If it's a Max node
	    if (state->is_max())
	    {
			int16_t value = INT16_MIN;
			for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
		    {
		        value = max(value, expect_minimax_alphabeta(&child, depth - 1, alpha, beta, chrono, action_method, max_gentil_groups, max_vilain_groups));
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
			for (State &child : state->successors(action_method, max_gentil_groups, max_vilain_groups))
		    {
		        value = min(value, expect_minimax_alphabeta(&child, depth - 1, alpha, beta, chrono, action_method, max_gentil_groups, max_vilain_groups));
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
