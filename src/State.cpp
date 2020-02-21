#include "../include/State.h"

#include <stdint.h>		/* int16_t */
#include <vector>
#include <utility>		/* std::pair */
#include <functional>	/* std::function */

#include "../include/utils.h"
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
	Map *map = copy_map(m_map);

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
	// Initialize vector
	std::vector<State> res;

	// If it's a chance node
	if (is_chance())
	{
		// Generate all possible outcomes for each battle in the map
		std::vector<Battle> battles = m_map->battles();
		const int n = battles.size();
		std::vector<std::vector<std::pair<Group*, double> > > outcomes(n);
		int *maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			outcomes[i] = m_map->battle_outcomes(battles[i]);
			maxes[i] = outcomes[i].size();
		}

		// Set lambda to generate every possible state
		std::function<void(int, int*)> lambda = [this, &outcomes, &res] (int n, int *arr) -> void
		{
			// Copy map
			Map *map = copy_map(m_map);

			// Remove battles
			map->remove_battles();

			// Add each winning group in the map and update the proba
			double proba = 1.0;
			for (int i = 0; i < n; ++i)
			{
				map->add_group(outcomes[i][arr[i]].first);
				proba *= outcomes[i][arr[i]].second;
			}

			// Add state in the vector
			res.push_back(State(map, m_turn, false, proba));
		};

		// Add every possible state
		nested_loops(n, maxes, lambda);

		// Free memory
		delete[] maxes;
	}

	// If it's a max/min node
	else
	{
		for (Action const& action : actions())
		{
			res.push_back(result(action));
		}
	}

	// Return states
	return res;
}

Map* copy_map(Map *map)
{
	Map *copy = 0;
	switch (METHOD)
	{
		// MapVectors
		case 0:
			copy = new MapVectors;
			break;

		// MapGrid
		case 1:
			copy = new MapGrid;
			break;

		default:
			break;
	}
	*copy = *map;
	return copy;
}
