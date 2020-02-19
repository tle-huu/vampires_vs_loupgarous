#include "../include/State.h"

#include <stdint.h>     /* int16_t */
#include <vector>

#include "../include/Point.h"
#include "../include/Group.h"
#include "../include/Map.h"
#include "../include/Action.h"

std::vector<Action> State::actions() const
{
    std::vector<Action> res;
    //TODO Generate all possible actions from a state
    return res;
}

State State::result(Action const& action) const
{
	// Copy map
	Map *map = 0;
	switch (METHOD)
	{
		// MapVectors
		case 0:
			map = new MapVectors;
			break;

		// MapGrid
		case 1:
			map = new MapGrid;
			break;

		default:
			break;
	}
	*map = *m_map;

	// Apply action to the map
	map->result(action);

	// Create state
	if (map->has_battle())
	{
		return State(map, !m_turn, true);
	}
	else
	{
		return State(map, !m_turn);
	}
}

std::vector<State> State::successors() const
{
	std::vector<State> res;
	if (is_chance())
	{
		//TODO Generate all possible outcomes from battles in the map
	}
	else
	{
		for (Action const& action : actions())
		{
			res.push_back(result(action));
		}
	}
	return res;
}

// deprecated
void build_tree(State &initial_state, int16_t depth)
{
    /*// Get groups of gentils
    std::vector<Gentil> gentils = initial_state.map().gentils();

    // For each group of gentils
    for (int gentil_index = 0; gentil_index < gentils.size(); ++gentil_index)
	{
		// Get gentil group
		Gentil gentil = gentils[gentil_index];
		int16_t x = gentil.position().x();
		int16_t y = gentil.position().y();
		int16_t n = gentil.number();

		// For each possible horizontal move
		for (int16_t dx = -1; dx <= 1; ++dx)
		{
			// For each possible vertical move
			for (int16_t dy = -1; dy <= 1; ++dy)
			{
				// Check if there is a move
				if (dx == 0 && dy == 0)
				{
					//continue;
				}

				// Get new position
				Point new_pos(x + dx, y + dy);

				// Check if the move is in-bounds
				if (initial_state.map().in_bounds(new_pos))
				{
					// For each possible number of units to move
					for (int16_t i = 1; i <= n; ++i)
					{
						Map new_map = initial_state.map();
						if (i == n)
						{
							new_map.gentils()[gentil_index].number(n - i);
						}
						else
						{
							new_map.gentils()[gentil_index].number(n - i);
						}
						
						Gentil new_group(new_pos, i);
						State new_state(new_map);
					}
				}
			}
		}
	}*/
}
