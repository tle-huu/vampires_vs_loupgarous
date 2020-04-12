#include "State.h"

#include <iostream>
#include <utility>		/* std::pair, std::move */
#include <functional>	/* std::function */
#include <algorithm>	/* std::find */

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

std::vector<Action> State::actions(int method, int16_t max_gentil_groups, int16_t max_vilain_groups) const
{
	// Get max groups
	int16_t max_groups = m_turn ? max_gentil_groups : max_vilain_groups;
	
	// Generate actions
	switch (method)
	{
	case 0:
		return actions_almost_full(max_groups);
		break;

	case 1:
		return actions_no_split(max_groups);
		break;

	case 2:
		return actions_limited_groups(max_groups);
		break;

	case 3:
		return actions_limited_groups_and_size(max_groups);
		break;

	case 4:
		return actions_customized(max_groups);
		break;

	default:
		return actions_limited_groups_and_size(max_groups);
		break;
	}
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

std::vector<State> State::successors(int method, int16_t max_gentil_groups, int16_t max_vilain_groups) const
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
		for (Action const& action : actions(method, max_gentil_groups, max_vilain_groups))
		{
			res.emplace_back(result(action));
		}
	}

	// Return states
	return res;
}

std::vector<Action> State::actions_almost_full(int16_t max_groups) const
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
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points](int n, int *arr) -> void
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
}

std::vector<Action> State::actions_no_split(int16_t max_groups) const
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
}

std::vector<Action> State::actions_limited_groups(int16_t max_groups) const
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
				maxes[i] = groups < max_groups ? numbers[i] : 1;
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
				maxes[i] = groups < max_groups ? numbers[i] : 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points, &numbers, &max_groups](int n, int *arr) -> void
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
			int16_t number = groups < max_groups ? arr[i] + 1 : numbers[i];

			// Get end point
			int16_t end_x = start_x + arr[i + groups] - 1;
			int16_t end_y = start_y + arr[i + 2 * groups] - 1;
			Point end_point(end_x, end_y);

			// Check if the group is actually moving
			if (start_point == end_point)
			{
				// Invalidate action if this action has already been generated
				if (groups < max_groups && number > 1)
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
				if (groups < max_groups && number < numbers[i] && !find_element<Point>(points, end_point))
				{
					if (++current_groups > max_groups)
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

std::vector<Action> State::actions_limited_groups_and_size(int16_t max_groups) const
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
		min_group_number = groups < max_groups ? m_map->min_group_number(m_turn) : 1;
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(gentils[i].position());
				numbers.push_back(gentils[i].number());
				maxes[i] = groups < max_groups ? numbers[i] - min(min_group_number, numbers[i]) + 1 : 1;
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
		min_group_number = groups < max_groups ? m_map->min_group_number(m_turn) : 1;
		n = 3 * groups;
		maxes = new int[n];
		for (int i = 0; i < n; ++i)
		{
			if (i < groups)
			{
				points.push_back(vilains[i].position());
				numbers.push_back(vilains[i].number());
				maxes[i] = groups < max_groups ? numbers[i] - min(min_group_number, numbers[i]) + 1 : 1;
			}
			else
			{
				maxes[i] = 3;
			}
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [this, &res, &groups, &points, &numbers, &max_groups, &min_group_number](int n, int *arr) -> void
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
			int16_t number = groups < max_groups ? arr[i] + min(min_group_number, numbers[i]) : numbers[i];

			// Get end point
			int16_t end_x = start_x + arr[i + groups] - 1;
			int16_t end_y = start_y + arr[i + 2 * groups] - 1;
			Point end_point(end_x, end_y);

			// Check if the group is actually moving
			if (start_point == end_point)
			{
				// Invalidate action if this action has already been generated
				if (groups < max_groups && number > min(min_group_number, numbers[i]))
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
				if (groups < max_groups && number < numbers[i] && !find_element<Point>(points, end_point))
				{
					if (++current_groups > max_groups)
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

std::vector<Action> State::actions_customized(int16_t max_groups) const
{
	// Initialize action vector
	std::vector<Action> res;
	
	// Get groups
	const std::vector<Gentil> gentils = m_map->gentils();
	const std::vector<Vilain> vilains = m_map->vilains();
	std::vector<Human> humans = m_map->humans();
	std::vector<Human> priority_humans;
	
	// Get priority humans
	int16_t min_diff = INT16_MAX;
	for (Human const& human : humans)
	{
		int16_t gentil_min_dist = human.min_distance(gentils);
		int16_t vilain_min_dist = human.min_distance(vilains);
		if (gentil_min_dist < INT16_MAX || vilain_min_dist < INT16_MAX)
		{
			int16_t diff = m_turn ? vilain_min_dist - gentil_min_dist : gentil_min_dist - vilain_min_dist;
			if (diff >= 0 && diff <= min_diff)
			{
				if (diff < min_diff)
				{
					priority_humans.clear();
				}
				priority_humans.push_back(human);
			}
		}
	}
	
	// Initialize moves
	int n = 0;
	int *maxes = 0;
	std::vector<std::vector<std::vector<Move> > > moves;

	// If it's gentil's turn
	if (m_turn)
	{
		n = gentils.size();
		maxes = new int[n];
		int idx = 0;
		
		// Initialize largest friendly group
		Gentil largest_gentil = m_map->get_largest_gentil();
		if (largest_gentil.number() == 1)
		{
			largest_gentil = Gentil();
		}
		
		// For each friend group
		for (Gentil const& gentil : gentils)
		{
			// Initialize this group moves
			std::vector<std::vector<Move> > group_moves;
			
			// Get interest points
			Point closest_gentil_point = closest_gentil(gentil, gentils);
			Point closest_human_point = closest_human(gentil, humans, true);
			Point closest_vilain_point = closest_vilain(gentil, vilains);
			
			// If there is at least one priority human
			if (!priority_humans.empty())
			{
				// Move the whole group to the closest priority human
				Point closest_priority_human_point = closest_human(gentil, priority_humans, true);
				if (closest_priority_human_point != Point::NULL_POINT)
				{
					Point to_priority_human = next_point(gentil.position(), closest_priority_human_point, closest_gentil_point);
					group_moves.push_back({ Move(gentil.position(), to_priority_human, gentil.number()) });
				}
				
				// Split the largest group
				if (n < max_groups && gentil == largest_gentil)
				{
					// Split between two priority humans
					if (priority_humans.size() > 1)
					{
						std::vector<Human> closest_priority_humans;
						for (Human const& human : priority_humans)
						{
							int16_t gentil_dist = distance(gentil, human);
							int16_t vilain_min_dist = human.min_distance(vilains);
							if (gentil_dist <= vilain_min_dist)
							{
								if (human.number() == gentil.number())
								{
									Point to_priority_human = next_point(gentil.position(), human.position(), closest_gentil_point);
									std::vector<Move> move{ Move(gentil.position(), to_priority_human, gentil.number()) };
									if (!find_element(group_moves, move))
									{
										group_moves.push_back(move);
									}
								}
								closest_priority_humans.push_back(human);
							}
						}
						int len = closest_priority_humans.size();
						if (len > 1)
						{
							Human priority_human1;
							Human priority_human2;
							for (int i = 0; i < len; ++i)
							{
								priority_human1 = closest_priority_humans[i];
								for (int j = i + 1; j < len; ++j)
								{
									priority_human2 = closest_priority_humans[j];
									int16_t number = gentil.number() - priority_human1.number() + priority_human2.number();
									if (number >= 0)
									{
										//for (int k = 0; k <= number; ++k)
										for (int k = 0; k <= number; k += number)
										{
											Point to_priority_human1 = next_point(gentil.position(), priority_human1.position(), closest_gentil_point);
											std::vector<Move> move{ Move(gentil.position(), to_priority_human1, priority_human1.number() + k) };
											Point to_priority_human2 = next_point(gentil.position(), priority_human2.position(), to_priority_human1);
											move.push_back(Move(gentil.position(), to_priority_human1, priority_human2.number() + number - k));
											if (!find_element(group_moves, move))
											{
												group_moves.push_back(move);
											}
										}
									}
								}
							}
						}
					}
					
					// Split between closest priority human and closest non priority human
					Human priority_human(closest_priority_human_point, m_map->get_number(closest_priority_human_point));
					Gentil split_gentil = gentil;
					split_gentil.add_number(-priority_human.number());
					remove_element(humans, priority_human);
					Point closest_non_priority_human_point = closest_human(split_gentil, humans, true);
					humans.push_back(priority_human);
					if (closest_non_priority_human_point != Point::NULL_POINT)
					{
						int16_t number = m_map->get_number(closest_non_priority_human_point);
						Point to_non_priority_human = next_point(gentil.position(), closest_non_priority_human_point, closest_gentil_point);
						std::vector<Move> move{ Move(gentil.position(), to_non_priority_human, number) };
						Point to_priority_human = next_point(gentil.position(), closest_priority_human_point, to_non_priority_human);
						move.push_back(Move(gentil.position(), to_priority_human, gentil.number() - number));
						if (!find_element(group_moves, move))
						{
							group_moves.push_back(move);
						}
					}
				}
			}
			
			// Move the whole group to the closest friendly group
			if (n > 1)
			{
				Point to_closest_gentil = next_point(gentil.position(), closest_gentil_point, closest_vilain_point);
				std::vector<Move> move{ Move(gentil.position(), to_closest_gentil, gentil.number()) };
				if (!find_element(group_moves, move))
				{
					group_moves.push_back(move);
				}
			}
			
			// Move the whole group to the closest human
			if (closest_human_point != Point::NULL_POINT)
			{
				Point to_closest_human = next_point(gentil.position(), closest_human_point, closest_vilain_point);
				std::vector<Move> move{ Move(gentil.position(), to_closest_human, gentil.number()) };
				if (!find_element(group_moves, move))
				{
					group_moves.push_back(move);
				}
			}
			
			// Move the whole group to the closest opponent
			Point to_closest_vilain = next_point(gentil.position(), closest_vilain_point);
			std::vector<Move> move{ Move(gentil.position(), to_closest_vilain, gentil.number()) };
			if (!find_element(group_moves, move))
			{
				group_moves.push_back(move);
			}
			
			// Move the whole group to the closest beatable opponent
			Point closest_beatable_vilain_point = closest_vilain(gentil, vilains, true);
			if (closest_beatable_vilain_point != Point::NULL_POINT)
			{
				Point to_closest_beatable_vilain = next_point(gentil.position(), closest_beatable_vilain_point);
				std::vector<Move> move2{ Move(gentil.position(), to_closest_beatable_vilain, gentil.number()) };
				if (to_closest_beatable_vilain != to_closest_vilain && !find_element(group_moves, move2))
				{
					group_moves.push_back(move2);
				}
			}
			
			// Add moves to the vector
			maxes[idx++] = n > 1 ? group_moves.size() + 1 : group_moves.size();
			moves.push_back(group_moves);
		}
	}

	// If it's vilain's turn
	else
	{
		n = vilains.size();
		maxes = new int[n];
		int idx = 0;
		
		// Initialize largest friendly group
		Vilain largest_vilain = m_map->get_largest_vilain();
		if (largest_vilain.number() == 1)
		{
			largest_vilain = Vilain();
		}

		// For each friend group
		for (Vilain const& vilain : vilains)
		{
			// Initialize this group moves
			std::vector<std::vector<Move> > group_moves;

			// Get interest points
			Point closest_vilain_point = closest_vilain(vilain, vilains);
			Point closest_human_point = closest_human(vilain, humans, true);
			Point closest_gentil_point = closest_gentil(vilain, gentils);

			// If there is at least one priority human
			if (!priority_humans.empty())
			{
				// Move the whole group to the closest priority human
				Point closest_priority_human_point = closest_human(vilain, priority_humans, true);
				if (closest_priority_human_point != Point::NULL_POINT)
				{
					Point to_priority_human = next_point(vilain.position(), closest_priority_human_point, closest_vilain_point);
					group_moves.push_back({ Move(vilain.position(), to_priority_human, vilain.number()) });
				}

				// Split the largest group
				if (n < max_groups && vilain == largest_vilain)
				{
					// Split between two priority humans
					if (priority_humans.size() > 1)
					{
						std::vector<Human> closest_priority_humans;
						for (Human const& human : priority_humans)
						{
							int16_t vilain_dist = distance(vilain, human);
							int16_t gentil_min_dist = human.min_distance(gentils);
							if (vilain_dist <= gentil_min_dist)
							{
								if (human.number() == vilain.number())
								{
									Point to_priority_human = next_point(vilain.position(), human.position(), closest_vilain_point);
									std::vector<Move> move{ Move(vilain.position(), to_priority_human, vilain.number()) };
									if (!find_element(group_moves, move))
									{
										group_moves.push_back(move);
									}
								}
								closest_priority_humans.push_back(human);
							}
						}
						int len = closest_priority_humans.size();
						if (len > 1)
						{
							Human priority_human1;
							Human priority_human2;
							for (int i = 0; i < len; ++i)
							{
								priority_human1 = closest_priority_humans[i];
								for (int j = i + 1; j < len; ++j)
								{
									priority_human2 = closest_priority_humans[j];
									int16_t number = vilain.number() - priority_human1.number() + priority_human2.number();
									if (number >= 0)
									{
										//for (int k = 0; k <= number; ++k)
										for (int k = 0; k <= number; k += number)
										{
											Point to_priority_human1 = next_point(vilain.position(), priority_human1.position(), closest_vilain_point);
											std::vector<Move> move{ Move(vilain.position(), to_priority_human1, priority_human1.number() + k) };
											Point to_priority_human2 = next_point(vilain.position(), priority_human2.position(), to_priority_human1);
											move.push_back(Move(vilain.position(), to_priority_human1, priority_human2.number() + number - k));
											if (!find_element(group_moves, move))
											{
												group_moves.push_back(move);
											}
										}
									}
								}
							}
						}
					}

					// Split between closest priority human and closest non priority human
					Human priority_human(closest_priority_human_point, m_map->get_number(closest_priority_human_point));
					Vilain split_vilain = vilain;
					split_vilain.add_number(-priority_human.number());
					remove_element(humans, priority_human);
					Point closest_non_priority_human_point = closest_human(split_vilain, humans, true);
					humans.push_back(priority_human);
					if (closest_non_priority_human_point != Point::NULL_POINT)
					{
						int16_t number = m_map->get_number(closest_non_priority_human_point);
						Point to_non_priority_human = next_point(vilain.position(), closest_non_priority_human_point, closest_vilain_point);
						std::vector<Move> move{ Move(vilain.position(), to_non_priority_human, number) };
						Point to_priority_human = next_point(vilain.position(), closest_priority_human_point, to_non_priority_human);
						move.push_back(Move(vilain.position(), to_priority_human, vilain.number() - number));
						if (!find_element(group_moves, move))
						{
							group_moves.push_back(move);
						}
					}
				}
			}

			// Move the whole group to the closest friendly group
			if (n > 1)
			{
				Point to_closest_vilain = next_point(vilain.position(), closest_vilain_point, closest_gentil_point);
				std::vector<Move> move{ Move(vilain.position(), to_closest_vilain, vilain.number()) };
				if (!find_element(group_moves, move))
				{
					group_moves.push_back(move);
				}
			}

			// Move the whole group to the closest human
			if (closest_human_point != Point::NULL_POINT)
			{
				Point to_closest_human = next_point(vilain.position(), closest_human_point, closest_gentil_point);
				std::vector<Move> move{ Move(vilain.position(), to_closest_human, vilain.number()) };
				if (!find_element(group_moves, move))
				{
					group_moves.push_back(move);
				}
			}

			// Move the whole group to the closest opponent
			Point to_closest_gentil = next_point(vilain.position(), closest_gentil_point);
			std::vector<Move> move{ Move(vilain.position(), to_closest_gentil, vilain.number()) };
			if (!find_element(group_moves, move))
			{
				group_moves.push_back(move);
			}

			// Move the whole group to the closest beatable opponent
			Point closest_beatable_gentil_point = closest_gentil(vilain, gentils, true);
			if (closest_beatable_gentil_point != Point::NULL_POINT)
			{
				Point to_closest_beatable_gentil = next_point(vilain.position(), closest_beatable_gentil_point);
				std::vector<Move> move2{ Move(vilain.position(), to_closest_beatable_gentil, vilain.number()) };
				if (to_closest_beatable_gentil != to_closest_gentil && !find_element(group_moves, move2))
				{
					group_moves.push_back(move2);
				}
			}

			// Add moves to the vector
			maxes[idx++] = n > 1 ? group_moves.size() + 1 : group_moves.size();
			moves.push_back(group_moves);
		}
	}

	// Set lambda to generate every possible action
	std::function<void(int, int*)> lambda = [&res, &moves](int n, int *arr) -> void
	{
		// Initialize action
		Action action;

		// Add moves of each group
		for (int i = 0; i < n; ++i)
		{
			if (arr[i] < moves[i].size())
			{
				action.add_moves(moves[i][arr[i]]);
			}
		}

		// Add action in the vector
		if (action.valid())
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
