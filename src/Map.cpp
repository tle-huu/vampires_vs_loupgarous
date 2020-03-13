#include "../include/Map.h"

#include <stdint.h>     /* int16_t */
#include <vector>
#include <utility>		/* std::pair */
#include <math.h>		/* round, pow */
#include <iostream>

#include "../include/utils.h"
#include "../include/Point.h"
#include "../include/Group.h"
#include "../include/Action.h"

int16_t METHOD = 0;

int16_t Map::heuristic() const
{
    int16_t value = 0;
    //TODO
    return value;
}

int16_t Map::utility() const
{
    return gentils_number() + heuristic();
}

std::vector<std::pair<Group*, double> > Map::battle_outcomes(Battle const& battle) const
{
	// Initialize vector
	std::vector<std::pair<Group*, double> > res;

	// Get number of units
	int E1 = battle.number_att();
	int E2 = battle.number_def();

	// Set probability that the attacker wins (for E1 <= E2)
	double P = (double) E1 / (double) (2 * E2);

	// For a battle against humans
	if (battle.defenders() == 'H')
	{
		// Attackers win and convert some humans
		for (int k = 0, n = E1 + E2; k <= n; ++k)
		{
			double proba = binomial_coef(n, k) * pow(P, k + 1) * pow(1 - P, n - k);
			Group *group = 0;
			if (battle.attackers() == 'G')
			{
				group = new Gentil(battle.position(), k);
			}
			else // battle.attackers() == 'V'
			{
				group = new Vilain(battle.position(), k);
			}
			std::pair<Group*, double> pair(group, proba);
			res.push_back(pair);
		}

		// Humans win
		for (int k = 0, n = E2; k <= n; ++k)
		{
			double proba = binomial_coef(n, k) * pow(1 - P, k + 1) * pow(P, n - k);
			Group *group = new Human(battle.position(), k);
			std::pair<Group*, double> pair(group, proba);
			res.push_back(pair);
		}
	}

	// For a battle against the opponent
	else
	{
		// Adjust P
		if (E1 > E2)
		{
			P = ((double) E1 / (double) E2) - 0.5;
		}

		// Attackers win
		for (int k = 0, n = E1; k <= n; ++k)
		{
			double proba = binomial_coef(n, k) * pow(P, k + 1) * pow(1 - P, n - k);
			Group *group = 0;
			if (battle.attackers() == 'G')
			{
				group = new Gentil(battle.position(), k);
			}
			else // battle.attackers() == 'V'
			{
				group = new Vilain(battle.position(), k);
			}
			std::pair<Group*, double> pair(group, proba);
			res.push_back(pair);
		}

		// Defenders win
		for (int k = 0, n = E2; k <= n; ++k)
		{
			double proba = binomial_coef(n, k) * pow(1 - P, k + 1) * pow(P, n - k);
			Group *group = 0;
			if (battle.defenders() == 'G')
			{
				group = new Gentil(battle.position(), k);
			}
			else // battle.defenders() == 'V'
			{
				group = new Vilain(battle.position(), k);
			}
			std::pair<Group*, double> pair(group, proba);
			res.push_back(pair);
		}
	}

	// Return outcomes
	return res;
}

void Map::result(Action const& action)
{
	
	for (Move const& move : action.moves())
	{
		result(move);
	}
	end_battles();
}

bool Map::in_bounds(Point const& point) const
{
	int16_t x = point.x();
	int16_t y = point.y();
	if (x < 0 || x >= m_columns || y < 0 || y >= m_lines)
	{
		return false;
	}
	return true;
}

void MapVectors::add_group(Group *group)
{
	char type = group->type();
	if (type == 'G' || type == 'V')
	{
		add_monster(type, group->position(), group->number());
	}
	else if (type == 'H')
	{
		add_human(group->position().x(), group->position().y(), group->number());
	}
	delete group;
}

int16_t MapVectors::gentils_number() const
{
	int16_t value = 0;
    for (Gentil const& gentil : m_gentils)
    {
        value += gentil.number();
    }
    return value;
}

void MapVectors::result(Move const& move)
{
	// Get group on the destination square
	Group *dest_group = get_group(move.end());

	// Get moving group
	Group *moving_group = get_group(move.start());

	// Update moving group
	int16_t dn = moving_group->number() - move.number();
	if (dn > 0)
	{
		moving_group->number(dn);
	}

	// If destination square is empty
	if (dest_group == NULL)
	{
		if (dn <= 0)
		{
			moving_group->position(move.end());
		}
		else
		{
			add_monster(moving_group->type(), move.end(), move.number());
		}
	}

	// If it's occupied by humans
	else if (dest_group->type() == 'H')
	{
		// If it's a winning battle
		if (move.number() >= dest_group->number())
		{
			if (dn <= 0)
			{
				moving_group->position(move.end());
				moving_group->add_number(dest_group->number());
			}
			else
			{
				add_monster(moving_group->type(), move.end(), dest_group->number() + move.number());
			}
		}
		else
		{
			add_battle(move.end(), moving_group->type(), move.number(), 'H', dest_group->number());
			if (dn <= 0)
			{
				remove_group(moving_group);
			}
		}
		remove_group(dest_group);
	}

	// If it's occupied by the same type of units
	else if (dest_group->type() == moving_group->type())
	{
		dest_group->add_number(move.number());
		if (dn <= 0)
		{
			remove_group(moving_group);
		}
	}

	// If it's occupied by the opponent
	else if (dest_group->type() == opponent(dest_group->type()))
	{
		// If it's a winning battle
		if (2 * move.number() >= 3 * dest_group->number())
		{
			if (dn <= 0)
			{
				moving_group->position(move.end());
			}
			else
			{
				add_monster(moving_group->type(), move.end(), move.number());
			}
		}
		else
		{
			add_battle(move.end(), moving_group->type(), move.number(), dest_group->type(), dest_group->number());
			if (dn <= 0)
			{
				remove_group(moving_group);
			}
		}
		remove_group(dest_group);
	}

	// If there is already a battle
	else
	{
		// Get new number of attackers
		Battle *battle = dynamic_cast<Battle*>(dest_group);
		int16_t n = battle->number_att() + move.number();

		// If it's a winning battle against humans
		if ( (battle->defenders() == 'H') && (n >= battle->number_def()) )
		{
			if (dn <= 0)
			{
				moving_group->position(move.end());
				moving_group->number(n + battle->number_def());
			}
			else
			{
				add_monster(moving_group->type(), move.end(), n + battle->number_def());
			}
			remove_group(dest_group);
		}

		// If it's a winning battle against opponents
		else if (2 * n >= 3 * battle->number_def())
		{
			if (dn <= 0)
			{
				moving_group->position(move.end());
				moving_group->number(n);
			}
			else
			{
				add_monster(moving_group->type(), move.end(), n);
			}
			remove_group(dest_group);
		}

		else
		{
			battle->add_attackers(move.number());
			if (dn <= 0)
			{
				remove_group(moving_group);
			}
		}
	}
}

void MapVectors::end_battles()
{
	for (Battle &battle : m_battles)
	{
		char def_type = battle.defenders();
		if ( (def_type == 'G' || def_type == 'V') && (2 * battle.number_def() >= 3 * battle.number_att()) )
		{
			remove_battle(battle);
			if (def_type == 'G')
			{
				add_gentil(battle.position().x(), battle.position().y(), battle.number_def());
			}
			else
			{
				add_vilain(battle.position().x(), battle.position().y(), battle.number_def());
			}
		}
	}
}

Group* MapVectors::get_group(Point const& pos)
{
	// Check in gentils
	for (Gentil &gentil : m_gentils)
	{
		if (gentil.position() == pos)
		{
			return &gentil;
		}
	}

	// Check in vilains
	for (Vilain &vilain : m_vilains)
	{
		if (vilain.position() == pos)
		{
			return &vilain;
		}
	}

	// Check in humans
	for (Human &human : m_humans)
	{
		if (human.position() == pos)
		{
			return &human;
		}
	}

	// Check in battles
	for (Battle &battle : m_battles)
	{
		if (battle.position() == pos)
		{
			return &battle;
		}
	}

	// Return a null pointer
	return NULL;
}

void MapVectors::add_battle(Point position, char attackers, int16_t number_att, char defenders, int16_t number_def)
{
	m_battles.push_back(Battle(position, attackers, number_att, defenders, number_def));
}

void MapVectors::add_monster(char type, Point const& pos, int16_t number)
{
	if (type == 'G')
	{
		add_gentil(pos.x(), pos.y(), number);
	}
	else // type == 'V'
	{
		add_vilain(pos.x(), pos.y(), number);
	}
}

void MapVectors::remove_gentil(Gentil &gentil)
{
	remove_element<Gentil>(m_gentils, gentil);
}

void MapVectors::remove_vilain(Vilain &vilain)
{
	remove_element<Vilain>(m_vilains, vilain);
}

void MapVectors::remove_human(Human &human)
{
	remove_element<Human>(m_humans, human);
}

void MapVectors::remove_battle(Battle &battle)
{
	remove_element<Battle>(m_battles, battle);
}

void MapVectors::remove_group(Group *group)
{
	// If it's a gentil's group
	if (group->type() == 'G')
	{
		Gentil *gentil = dynamic_cast<Gentil*>(group);
		remove_gentil(*gentil);
	}

	// If it's a vilain's group
	else if (group->type() == 'V')
	{
		Vilain *vilain = dynamic_cast<Vilain*>(group);
		remove_vilain(*vilain);
	}

	// If it's a human's group
	else if (group->type() == 'H')
	{
		Human *human = dynamic_cast<Human*>(group);
		remove_human(*human);
	}

	// If it's a battle
	else
	{
		Battle *battle = dynamic_cast<Battle*>(group);
		remove_battle(*battle);
	}
}

Cell MapVectors::getCell(int x, int y) const{
	
}

MapGrid::MapGrid(int16_t lines, int16_t columns)
	: Map(lines, columns)
{
	// Initialize grid
	m_grid = new Square*[m_lines];
    for (int16_t i = 0; i < m_lines; ++i)
    {
        m_grid[i] = new Square[m_columns];
        for (int16_t j = 0; j < m_columns; ++j)
    	{
    		m_grid[i][j] = Square{};
    	}
    }
}

MapGrid::~MapGrid()
{
	for (int16_t i = 0; i < m_lines; ++i)
	{
		delete[] m_grid[i];
	}
	delete[] m_grid;
}

std::vector<Gentil> MapGrid::gentils() const
{
	std::vector<Gentil> res;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'G')
			{
				res.push_back(Gentil(Point(x, y), get_number(x, y)));
			}
		}
	}
	return res;
}

std::vector<Vilain> MapGrid::vilains() const
{
	std::vector<Vilain> res;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'V')
			{
				res.push_back(Vilain(Point(x, y), get_number(x, y)));
			}
		}
	}
	return res;
}

std::vector<Human> MapGrid::humans() const
{
	std::vector<Human> res;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'H')
			{
				res.push_back(Human(Point(x, y), get_number(x, y)));
			}
		}
	}
	return res;
}

std::vector<Battle> MapGrid::battles() const
{
	std::vector<Battle> res;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'B')
			{
				res.push_back(Battle(Point(x, y), get_att(x, y), get_number(x, y, false), get_def(x, y), get_number(x, y)));
			}
		}
	}
	return res;
}

void MapGrid::add_gentil(int16_t x, int16_t y, int16_t n)
{
	set_group(x, y, 'G', n);
}

void MapGrid::add_vilain(int16_t x, int16_t y, int16_t n)
{
	set_group(x, y, 'V', n);
}

void MapGrid::add_human(int16_t x, int16_t y, int16_t n)
{
	set_group(x, y, 'H', n);
}

void MapGrid::add_group(Group *group)
{
	set_group(group->position().x(), group->position().y(), group->type(), group->number());
	delete group;
}

bool MapGrid::has_battle() const
{
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'B')
			{
				return true;
			}
		}
	}
	return false;
}

void MapGrid::remove_battles()
{
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'B')
			{
				remove_group(x, y);
			}
		}
	}
}

bool MapGrid::is_terminal() const
{
	int gentils_counter = 0, vilains_counter = 0;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'G')
			{
				++gentils_counter;
			}
			else if (get_type(x, y) == 'V')
			{
				++vilains_counter;
			}
		}
	}
	return (gentils_counter == 0) || (vilains_counter == 0);
}

int16_t MapGrid::gentils_number() const
{
	int16_t value = 0;
    for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'G')
			{
				value += get_number(x, y);
			}
		}
	}
	return value;
}

void MapGrid::result(Move const& move)
{
	// Get start position
	int16_t x = move.start().x();
	int16_t y = move.start().y();

	// Get moving group type
	char group_type = get_type(x, y);

	// Update moving group number
	set_number(x, y, get_number(x, y) - move.number());

	// Get end position
	x = move.end().x();
	y = move.end().y();

	// Get destination square type
	char dest_type = get_type(x, y);

	// Set destination square

	// If destination square is empty
	if (dest_type == 'E')
	{
		set_group(x, y, group_type, move.number());
	}

	// If it's occupied by humans
	else if (dest_type == 'H')
	{
		// If it's a winning battle
		if (move.number() >= get_number(x, y))
		{
			set_group(x, y, group_type, get_number(x, y) + move.number());
		}
		else
		{
			set_battle(x, y, group_type, move.number());
		}
	}

	// If it's occupied by the same type of units
	else if (dest_type == group_type)
	{
		add_number(x, y, move.number());
	}

	// If it's occupied by the opponent
	else if (dest_type == opponent(dest_type))
	{
		// If it's a winning battle
		if (2 * move.number() >= 3 * get_number(x, y))
		{
			set_group(x, y, group_type, move.number());
		}
		else
		{
			set_battle(x, y, group_type, move.number());
		}
	}

	// If there is already a battle
	else
	{
		// Get new number of attackers
		int16_t n = get_number(x, y, false) + move.number();

		// If it's a winning battle against humans
		if ( (get_def(x, y) == 'H') && (n >= get_number(x, y)) )
		{
			set_group(x, y, group_type, get_number(x, y) + n);
		}

		// If it's a winning battle against opponents
		else if (2 * n >= 3 * get_number(x, y))
		{
			set_group(x, y, group_type, n);
		}

		else
		{
			add_attackers(x, y, move.number());
		}
	}
}

void MapGrid::end_battles()
{
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'B')
			{
				if ( (get_def(x, y) == 'G' || get_def(x, y) == 'V') && (2 * get_number(x, y) >= 3 * get_number(x, y, false)) )
				{
					set_group(x, y, get_def(x, y), get_number(x, y));
				}
			}
		}
	}
}

void MapGrid::set_group(int16_t x, int16_t y, char type, int16_t n)
{
	m_grid[y][x].type = type;
	m_grid[y][x].number = n;
}

void MapGrid::set_battle(int16_t x, int16_t y, char att, int16_t number_att)
{
	m_grid[y][x].def = m_grid[y][x].type;
	m_grid[y][x].type = 'B';
	m_grid[y][x].att = att;
	m_grid[y][x].number_att = number_att;
}

Cell MapGrid::getCell(int x, int y) const{
	Cell c;
	c.element = this->m_grid[y][x].type;
	c.number = this->m_grid[y][x].number;
	return c;
}