#include "Map.h"

#include <math.h>		/* round, pow */

#include "utils.h"

const int16_t MAP_METHOD = 1;

int16_t Map::heuristic(bool turn) const
{
    int16_t value = 0;
	int16_t gentils_humans = 0, vilains_humans = 0;
	for (Human const& human : humans())
	{
		int16_t gentil_min_dist = human.min_distance(gentils());
		int16_t vilain_min_dist = human.min_distance(vilains());
		if (gentil_min_dist < INT16_MAX || vilain_min_dist < INT16_MAX)
		{
			if (turn)
			{
				gentil_min_dist <= vilain_min_dist ? gentils_humans += human.number() : vilains_humans += human.number();
			}
			else
			{
				gentil_min_dist < vilain_min_dist ? gentils_humans += human.number() : vilains_humans += human.number();
			}
		}
	}
	return gentils_humans - vilains_humans;
}

int16_t Map::utility(bool turn) const
{
    if (is_terminal())
    {
		int16_t value = 0;
		for (Human const& human : humans())
		{
			value += human.number();
		}
		value += gentils_number() + 1;
		if (gentils_number() == 0)
    	{
			if (vilains_number() == 0)
			{
				return 0;
			}
			return -value;
    	}
    	else // vilains_number() == 0
    	{
			return value;
    	}
    }
	int16_t value = heuristic(turn);
	int16_t gentils_num = gentils_number(), vilains_num = vilains_number();
	if (2 * gentils_num >= 3 * vilains_num)
	{
		value += gentils_num;
	}
	else if (2 * vilains_num >= 3 * gentils_num)
	{
		value -= vilains_num;
	}
	else
	{
		value += 2 * (gentils_number() - vilains_number());
	}
	return value;
}

int16_t Map::min_group_number(bool turn) const
{
	int16_t min_number = INT16_MAX;
	for (Human const& human : humans())
	{
		min_number = min(min_number, human.number());
	}
	if (turn)
	{
		for (Vilain const& vilain : vilains())
		{
			min_number = min(min_number, (int16_t) round(1.5 * (double) vilain.number()));
		}
	}
	else
	{
		for (Gentil const& gentil : gentils())
		{
			min_number = min(min_number, (int16_t)round(1.5 * (double)gentil.number()));
		}
	}
	return min_number;
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

	// Initialize no survival probability
	double no_survival_proba = 0;

	// For a battle against humans
	if (battle.defenders() == 'H')
	{
		// Attackers win and convert some humans
		for (int k = 0, n = E1 + E2; k <= n; ++k)
		{
			double proba = binomial_coef(n, k) * pow(P, k + 1) * pow(1 - P, n - k);
			if (k == 0)
			{
				no_survival_proba = proba;
				continue;
			}
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
			if (k == 0)
			{
				proba += no_survival_proba;
			}
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
			if (k == 0)
			{
				no_survival_proba = proba;
				continue;
			}
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
			if (k == 0)
			{
				proba += no_survival_proba;
			}
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

bool Map::preferable_than(Map *map) const
{
	// Check if map is not null pointer
	if (map == nullptr)
	{
		return true;
	}
	
	// Get populations
	int16_t self_gentils_number = gentils_number();
	int16_t other_gentils_number = map->gentils_number();
	int16_t self_vilains_number = vilains_number();
	int16_t other_vilains_number = map->vilains_number();

	// Choose the winning one
	if (other_vilains_number == 0 && other_gentils_number > 0)
	{
		return false;
	}
	if (self_vilains_number == 0 && self_gentils_number > 0)
	{
		return true;
	}
	
	// Choose the one with the biggest population ratio
	double self_population_ratio = ((double) self_gentils_number) / ((double) self_vilains_number);
	double other_population_ratio = ((double) other_gentils_number) / ((double) other_vilains_number);
	if (self_population_ratio != other_population_ratio)
	{
		return self_population_ratio > other_population_ratio;
	}

	// Choose the one with the more gentils groups
	std::vector<Gentil> self_gentils = gentils();
	std::vector<Gentil> other_gentils = map->gentils();
	if (self_gentils.size() != other_gentils.size())
	{
		return self_gentils.size() > other_gentils.size();
	}

	// Choose the one minimizing the smallest distance to a human group
	std::vector<Human> human_groups = humans();
	int16_t self_min_dist = INT16_MAX;
	for (Human const& human : human_groups)
	{
		self_min_dist = min(self_min_dist, human.min_distance(self_gentils));
	}
	human_groups = map->humans();
	int16_t other_min_dist = INT16_MAX;
	for (Human const& human : human_groups)
	{
		other_min_dist = min(other_min_dist, human.min_distance(other_gentils));
	}
	if (self_min_dist != other_min_dist)
	{
		return self_min_dist < other_min_dist;
	}

	// Choose the one minimizing the smallest distance to a vilain group
	std::vector<Vilain> vilain_groups = vilains();
	self_min_dist = INT16_MAX;
	for (Vilain const& vilain : vilain_groups)
	{
		self_min_dist = min(self_min_dist, vilain.min_distance(self_gentils));
	}
	vilain_groups = map->vilains();
	other_min_dist = INT16_MAX;
	for (Vilain const& vilain : vilain_groups)
	{
		other_min_dist = min(other_min_dist, vilain.min_distance(other_gentils));
	}
	return self_min_dist < other_min_dist;
}

std::ostream& operator<<(std::ostream &out, Map *map)
{
	out << map->to_string();
	return out;
}

char opponent(char type)
{
	return type == 'G' ? 'V' : 'G';
}

void MapVectors::add_group(Group *group)
{
	if (group->number() <= 0)
	{
		return;
	}
	char type = group->type();
	if (type == 'G' || type == 'V')
	{
		add_monster(type, group->position(), group->number());
	}
	else if (type == 'H')
	{
		add_human(group->pos_x(), group->pos_y(), group->number());
	}
}

void MapVectors::remove_group(int16_t x, int16_t y)
{
	for (Gentil &gentil : m_gentils)
	{
		if (gentil.pos_x() == x && gentil.pos_y() == y)
		{
			remove_gentil(gentil);
		}
	}

	for (Vilain &vilain : m_vilains)
	{
		if (vilain.pos_x() == x && vilain.pos_y() == y)
		{
			remove_vilain(vilain);
		}
	}

	for (Human &human : m_humans)
	{
		if (human.pos_x() == x && human.pos_y() == y)
		{
			remove_human(human);
		}
	}
}

int16_t MapVectors::get_number(Point const& point) const
{
	for (Gentil const& gentil : m_gentils)
	{
		if (gentil.position() == point)
		{
			return gentil.number();
		}
	}
	for (Vilain const& vilain : m_vilains)
	{
		if (vilain.position() == point)
		{
			return vilain.number();
		}
	}
	for (Human const& human : m_humans)
	{
		if (human.position() == point)
		{
			return human.number();
		}
	}
	return 0;
}

Gentil MapVectors::get_largest_gentil() const
{
	int16_t max_number = 0;
	Gentil largest_gentil;
	for (Gentil const& gentil : m_gentils)
	{
		int16_t number = gentil.number();
		if (number > max_number)
		{
			max_number = number;
			largest_gentil = gentil;
		}
	}
	return largest_gentil;
}

Vilain MapVectors::get_largest_vilain() const
{
	int16_t max_number = 0;
	Vilain largest_vilain;
	for (Vilain const& vilain : m_vilains)
	{
		int16_t number = vilain.number();
		if (number > max_number)
		{
			max_number = number;
			largest_vilain = vilain;
		}
	}
	return largest_vilain;
}

std::string MapVectors::to_string() const
{
	return "Not implemented yet";
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

int16_t MapVectors::vilains_number() const
{
	int16_t value = 0;
    for (Vilain const& vilain : m_vilains)
    {
        value += vilain.number();
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
			if (def_type == 'G')
			{
				add_gentil(battle.pos_x(), battle.pos_y(), battle.number_def());
			}
			else
			{
				add_vilain(battle.pos_x(), battle.pos_y(), battle.number_def());
			}
			remove_battle(battle);
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
	return nullptr;
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

void MapGrid::add_group(Group *group)
{
	group->number() <= 0 ? remove_group(group->pos_x(), group->pos_y()) : set_group(group->pos_x(), group->pos_y(), group->type(), group->number());
}

Gentil MapGrid::get_largest_gentil() const
{
	int16_t max_number = 0;
	Gentil largest_gentil;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'G')
			{
				int16_t number = get_number(x, y);
				if (number > max_number)
				{
					max_number = number;
					largest_gentil = Gentil(Point(x, y), number);
				}
			}
		}
	}
	return largest_gentil;
}

Vilain MapGrid::get_largest_vilain() const
{
	int16_t max_number = 0;
	Vilain largest_vilain;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'V')
			{
				int16_t number = get_number(x, y);
				if (number > max_number)
				{
					max_number = number;
					largest_vilain = Vilain(Point(x, y), number);
				}
			}
		}
	}
	return largest_vilain;
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
	bool no_gentils = true, no_vilains = true;
	for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'G')
			{
				no_gentils = false;
			}
			else if (get_type(x, y) == 'V')
			{
				no_vilains = false;
			}
		}
	}
	return (no_gentils || no_vilains);
}

std::string MapGrid::to_string() const
{
	std::string str = "Map:\n";
	for (int16_t y = -1; y < m_lines; ++y)
	{
		for (int16_t x = -1; x < m_columns; ++x)
		{
			if (x < 0)
			{
				str = y < 0 ? str + " " : str + int_to_string(y);
			}
			else if (y < 0)
			{
				str = str + int_to_string(x);
			}
			else if (get_type(x, y) != 'E')
			{
				str = str + get_type(x, y) + int_to_string(get_number(x, y));
			}
			else
			{
				str = str + "-";
			}
			str = str + "\t";
		}
		str = str + "\n";
	}
	return str;
}

MapGrid::MapGrid(MapGrid const& map)
{
	// Initialize size
	m_lines = map.lines();
	m_columns = map.columns();

	// Initialize grid
	m_grid = new Square*[m_lines];
	for (int16_t i = 0; i < m_lines; ++i)
	{
		m_grid[i] = new Square[m_columns];
		for (int16_t j = 0; j < m_columns; ++j)
		{
			m_grid[i][j] = Square{map.m_grid[i][j].type, map.m_grid[i][j].number};
		}
	}
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

int16_t MapGrid::vilains_number() const
{
	int16_t value = 0;
    for (int16_t y = 0; y < m_lines; ++y)
	{
		for (int16_t x = 0; x < m_columns; ++x)
		{
			if (get_type(x, y) == 'V')
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
	else if (dest_type == opponent(group_type))
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
		if ((get_def(x, y) == 'H') && (n >= get_number(x, y)))
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
				if ((get_def(x, y) == 'G' || get_def(x, y) == 'V') && (2 * get_number(x, y) >= 3 * get_number(x, y, false)))
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
