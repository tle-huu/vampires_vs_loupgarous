#include "State.h"

#include <utility>		/* std::pair, std::move */
#include <functional>	/* std::function */

#include "utils.h"
#include "Point.h"
#include "Group.h"

State::State(State &&state)
{
	m_map = state.m_map;
	m_turn = state.m_turn;
	m_chance = state.m_chance;
	m_proba = state.m_proba;
	state.m_map = nullptr;
}

State& State::operator=(State const& state)
{
	if (this != &state)
	{
		m_map = state.m_map->clone();
		m_turn = state.m_turn;
		m_chance = state.m_chance;
		m_proba = state.m_proba;
	}
	return *this;
}

/*std::vector<Action> State::actions() const
{
	// Initialize vector
	std::vector<Action> res;

	// Get all positions from groups that can move
	int groups;
	int n;
	int *maxes = 0;
	std::vector<Point> points;
	if (m_turn)
	{
		std::vector<Gentil> gentils = m_map->gentils();
		groups = gentils.size();
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(gentils[i].position());
				maxes[i] = gentils[i].number();
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}
	else
	{
		std::vector<Vilain> vilains = m_map->vilains();
		groups = vilains.size();
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(vilains[i].position());
				maxes[i] = vilains[i].number();
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points] (int n, int *arr) -> void
	{
		// Initialize action
		Action action;

		// Set validation variables
		bool valid = true;
		std::vector<Point> start_points;

		// For each group
		for (int i = 0; i < groups; ++i)
		{
			// Get start point
			Point start_point = points[i];
			int16_t start_x = start_point.x();
			int16_t start_y = start_point.y();

			// Get number of units moving
			int16_t number = arr[i] + 1;

			// Get end point
			int16_t end_x = start_x + arr[i + groups] - 1;
			int16_t end_y = start_y + arr[i + 2 * groups] - 1;
			Point end_point(end_x, end_y);

			// Check if the group is actually moving
			if (start_point == end_point)
			{
				// Invalidate action if this action has already been generated
				if (number > 1)
				{
					valid = false;
					break;
				}
			}
			else
			{
				// Invalidate action if end point is also a start point
				// or if end point is not in map bounds
				if (find_element<Point>(start_points, end_point) || !m_map->in_bounds(end_point))
				{
					valid = false;
					break;
				}

				// Update start points vector
				start_points.push_back(start_point);

				// Add move in action
				Move move(start_x, start_y, number, end_x, end_y);
				action.add_move(move);
			}
		}
		
		// Add action in the vector
		if (valid && !action.empty())
		{
			res.push_back(action);
		}
	};
	
	// Add every possible action
	nested_loops(n, maxes, lambda);

	// Free memory
	delete[] maxes;
	
	// Return actions
	return res;
}*/

/*std::vector<Action> State::actions() const
{
	// Initialize vector
	std::vector<Action> res;

	// Get all positions from groups that can move
	int groups;
	int n;
	int *maxes = 0;
	std::vector<Point> points;
	std::vector<int16_t> numbers;
	if (m_turn)
	{
		std::vector<Gentil> gentils = m_map->gentils();
		groups = gentils.size();
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(gentils[i].position());
				numbers.push_back(gentils[i].number());
				maxes[i] = 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}
	else
	{
		std::vector<Vilain> vilains = m_map->vilains();
		groups = vilains.size();
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(vilains[i].position());
				numbers.push_back(vilains[i].number());
				maxes[i] = 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points, &numbers](int n, int *arr) -> void
	{
		// Initialize action
		Action action;

		// Set validation variables
		bool valid = true;
		std::vector<Point> start_points;

		// For each group
		for (int i = 0; i < groups; ++i)
		{
			// Get start point
			Point start_point = points[i];
			int16_t start_x = start_point.x();
			int16_t start_y = start_point.y();

			// Get number of units moving
			int16_t number = numbers[i];

			// Get end point
			int16_t end_x = start_x + arr[i + groups] - 1;
			int16_t end_y = start_y + arr[i + 2 * groups] - 1;
			Point end_point(end_x, end_y);

			// Check if the group is actually moving
			if (start_point != end_point)
			{
				// Invalidate action if end point is also a start point
				// or if end point is not in map bounds
				if (find_element<Point>(start_points, end_point) || !m_map->in_bounds(end_point))
				{
					valid = false;
					break;
				}

				// Update start points vector
				start_points.push_back(start_point);

				// Add move in action
				Move move(start_x, start_y, number, end_x, end_y);
				action.add_move(move);
			}
		}

		// Add action in the vector
		if (valid && !action.empty())
		{
			res.push_back(action);
		}
	};

	// Add every possible action
	nested_loops(n, maxes, lambda);

	// Free memory
	delete[] maxes;

	// Return actions
	return res;
}*/

/*std::vector<Action> State::actions() const
{
	// Initialize vector
	std::vector<Action> res;

	// Get all positions from groups that can move
	int groups;
	int n;
	int *maxes = 0;
	std::vector<Point> points;
	std::vector<int16_t> numbers;
	if (m_turn)
	{
		std::vector<Gentil> gentils = m_map->gentils();
		groups = gentils.size();
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(gentils[i].position());
				numbers.push_back(gentils[i].number());
				maxes[i] = groups < MAX_GROUPS ? numbers[i] : 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}
	else
	{
		std::vector<Vilain> vilains = m_map->vilains();
		groups = vilains.size();
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(vilains[i].position());
				numbers.push_back(vilains[i].number());
				maxes[i] = groups < MAX_GROUPS ? numbers[i] : 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points, &numbers](int n, int *arr) -> void
	{
		// Initialize action
		Action action;

		// Set validation variables
		bool valid = true;
		std::vector<Point> start_points;
		int current_groups = groups;

		// For each group
		for (int i = 0; i < groups; ++i)
		{
			// Get start point
			Point start_point = points[i];
			int16_t start_x = start_point.x();
			int16_t start_y = start_point.y();

			// Get number of units moving
			int16_t number = groups < MAX_GROUPS ? arr[i] + 1 : numbers[i];

			// Get end point
			int16_t end_x = start_x + arr[i + groups] - 1;
			int16_t end_y = start_y + arr[i + 2 * groups] - 1;
			Point end_point(end_x, end_y);

			// Check if the group is actually moving
			if (start_point == end_point)
			{
				// Invalidate action if this action has already been generated
				if (groups < MAX_GROUPS && number > 1)
				{
					valid = false;
					break;
				}
			}
			else
			{
				// Invalidate action if end point is also a start point
				// or if end point is not in map bounds
				if (find_element<Point>(start_points, end_point) || !m_map->in_bounds(end_point))
				{
					valid = false;
					break;
				}
				
				// Invalidate action if there are more groups than MAX_GROUPS
				if (groups < MAX_GROUPS && number < numbers[i] && !find_element<Point>(points, end_point))
				{
					if (++current_groups > MAX_GROUPS)
					{
						valid = false;
						break;
					}
				}

				// Update start points vector
				start_points.push_back(start_point);

				// Add move in action
				Move move(start_x, start_y, number, end_x, end_y);
				action.add_move(move);
			}
		}

		// Add action in the vector
		if (valid && !action.empty())
		{
			res.push_back(action);
		}
	};

	// Add every possible action
	nested_loops(n, maxes, lambda);

	// Free memory
	delete[] maxes;

	// Return actions
	return res;
}*/

std::vector<Action> State::actions() const
{
	// Initialize vector
	std::vector<Action> res;

	// Get all positions from groups that can move
	int groups;
	int n;
	int *maxes = 0;
	std::vector<Point> points;
	std::vector<int16_t> numbers;
	int16_t min_group_number;
	if (m_turn)
	{
		std::vector<Gentil> gentils = m_map->gentils();
		groups = gentils.size();
		min_group_number = groups < MAX_GROUPS ? m_map->min_group_number(m_turn) : 1;
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(gentils[i].position());
				numbers.push_back(gentils[i].number());
				maxes[i] = groups < MAX_GROUPS ? numbers[i] - min(min_group_number, numbers[i]) + 1 : 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}
	else
	{
		std::vector<Vilain> vilains = m_map->vilains();
		groups = vilains.size();
		min_group_number = groups < MAX_GROUPS ? m_map->min_group_number(m_turn) : 1;
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(vilains[i].position());
				numbers.push_back(vilains[i].number());
				maxes[i] = groups < MAX_GROUPS ? numbers[i] - min(min_group_number, numbers[i]) + 1 : 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points, &numbers, &min_group_number](int n, int *arr) -> void
	{
		// Initialize action
		Action action;

		// Set validation variables
		bool valid = true;
		std::vector<Point> start_points;
		int current_groups = groups;

		// For each group
		for (int i = 0; i < groups; ++i)
		{
			// Get start point
			Point start_point = points[i];
			int16_t start_x = start_point.x();
			int16_t start_y = start_point.y();

			// Get number of units moving
			int16_t number = groups < MAX_GROUPS ? arr[i] + min(min_group_number, numbers[i]) : numbers[i];

			// Get end point
			int16_t end_x = start_x + arr[i + groups] - 1;
			int16_t end_y = start_y + arr[i + 2 * groups] - 1;
			Point end_point(end_x, end_y);

			// Check if the group is actually moving
			if (start_point == end_point)
			{
				// Invalidate action if this action has already been generated
				if (groups < MAX_GROUPS && number > min(min_group_number, numbers[i]))
				{
					valid = false;
					break;
				}
			}
			else
			{
				// Invalidate action if end point is also a start point
				// or if end point is not in map bounds
				if (find_element<Point>(start_points, end_point) || !m_map->in_bounds(end_point))
				{
					valid = false;
					break;
				}

				// Invalidate action if there are more groups than MAX_GROUPS
				if (groups < MAX_GROUPS && number < numbers[i] && !find_element<Point>(points, end_point))
				{
					if (++current_groups > MAX_GROUPS)
					{
						valid = false;
						break;
					}
				}

				// Update start points vector
				start_points.push_back(start_point);

				// Add move in action
				Move move(start_x, start_y, number, end_x, end_y);
				action.add_move(move);
			}
		}

		// Add action in the vector
		if (valid && !action.empty())
		{
			res.push_back(action);
		}
	};

	// Add every possible action
	nested_loops(n, maxes, lambda);

	// Free memory
	delete[] maxes;

	// Return actions
	return res;
}

State State::result(Action const& action) const
{
	// Clone map
	Map *map = m_map->clone();

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
			// Clone map
			Map *map = m_map->clone();

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
			res.emplace_back(map, m_turn, false, proba);
		};

		// Add every possible state
		nested_loops(n, maxes, lambda);

		// Free memory
		delete[] maxes;
		for (int i = 0; i < n; ++i)
		{
			for (std::pair<Group*, double> &pair : outcomes[i])
			{
				delete pair.first;
			}
		}
	}

	// If it's a max/min node
	else
	{
		for (Action const& action : actions())
		{
			res.emplace_back(result(action));
		}
	}

	// Return states
	return res;
}
