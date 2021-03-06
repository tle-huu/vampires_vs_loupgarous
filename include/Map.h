#ifndef Map_h_INCLUDED
#define Map_h_INCLUDED

#include <stdint.h>     /* int16_t */
#include <string>
#include <iostream>
#include <vector>
#include <utility>      /* std::pair */

#include "Point.h"
#include "Group.h"
#include "Action.h"

// Map implementation : 0 for MapVectors, 1 for MapGrid
extern const int16_t MAP_METHOD;

/**
 * An interface to represent a map of the game
 */
class Map
{
public:

	Map() : m_lines(0), m_columns(0) {}
	Map(int16_t lines, int16_t columns) : m_lines(lines), m_columns(columns) {}

    virtual ~Map() {}

	virtual Map* clone() const = 0;

    int16_t lines() const { return m_lines; }
    int16_t columns() const { return m_columns; }

    virtual std::vector<Gentil> gentils() const = 0;
    virtual std::vector<Vilain> vilains() const = 0;
    virtual std::vector<Human> humans() const = 0;
    virtual std::vector<Battle> battles() const = 0;

    virtual void add_gentil(int16_t x, int16_t y, int16_t n) = 0;
    virtual void add_vilain(int16_t x, int16_t y, int16_t n) = 0;
    virtual void add_human(int16_t x, int16_t y, int16_t n) = 0;
    virtual void add_group(Group *group) = 0; // not for battle groups

	virtual void remove_group(int16_t x, int16_t y) = 0;

    int16_t heuristic(bool turn) const;
    int16_t utility(bool turn) const;

	virtual int16_t get_number(Point const& point) const = 0;
	int16_t min_group_number(bool turn) const;

	virtual Gentil get_largest_gentil() const = 0;
	virtual Vilain get_largest_vilain() const = 0;

    std::vector<std::pair<Group*, double> > battle_outcomes(Battle const& battle) const;

    virtual bool has_battle() const = 0;
    virtual void remove_battles() = 0;

    virtual bool is_terminal() const = 0;

    void result(Action const& action);

    bool in_bounds(Point const& point) const;

	bool preferable_than(Map *map) const;

	virtual std::string to_string() const = 0;


protected:

    virtual int16_t gentils_number() const = 0;
    virtual int16_t vilains_number() const = 0;

    virtual void result(Move const& move) = 0;

    virtual void end_battles() = 0;

    int16_t m_lines;
    int16_t m_columns;
};

std::ostream& operator<<(std::ostream &out, Map *map);

char opponent(char type);

/**
 * An implementation of Map represented by vectors of groups
 */
class MapVectors : public Map
{
public:

	MapVectors() : Map() {}
    MapVectors(int16_t lines, int16_t columns) : Map(lines, columns) {}

	MapVectors* clone() const { return new MapVectors(*this); }

    std::vector<Gentil> gentils() const { return m_gentils; }
    std::vector<Vilain> vilains() const { return m_vilains; }
    std::vector<Human> humans() const { return m_humans; }
    std::vector<Battle> battles() const { return m_battles; }

    void add_gentil(int16_t x, int16_t y, int16_t n) { m_gentils.push_back(Gentil(Point(x, y), n)); }
    void add_vilain(int16_t x, int16_t y, int16_t n) { m_vilains.push_back(Vilain(Point(x, y), n)); }
    void add_human(int16_t x, int16_t y, int16_t n) { m_humans.push_back(Human(Point(x, y), n)); }
    void add_group(Group *group);

	void remove_group(int16_t x, int16_t y);

	int16_t get_number(Point const& point) const;

	Gentil get_largest_gentil() const;
	Vilain get_largest_vilain() const;

    bool has_battle() const { return !m_battles.empty(); }
    void remove_battles() { m_battles.clear(); }

    bool is_terminal() const { return (m_gentils.size() == 0) || (m_vilains.size() == 0); }

	std::string to_string() const;


private:

    int16_t gentils_number() const;
    int16_t vilains_number() const;

    void result(Move const& move);

    void end_battles();

    Group* get_group(Point const& pos);
    void add_battle(Point position, char attackers, int16_t number_att, char defenders, int16_t number_def);
    void add_monster(char type, Point const& pos, int16_t number); // only for gentil's and vilain's groups
    void remove_gentil(Gentil &gentil);
    void remove_vilain(Vilain &vilain);
    void remove_human(Human &human);
    void remove_battle(Battle &battle);
    void remove_group(Group *group);

    std::vector<Gentil> m_gentils;
    std::vector<Vilain> m_vilains;
    std::vector<Human> m_humans;
    std::vector<Battle> m_battles;
};

/**
 * An implementation of Map represented by a grid
 */
class MapGrid : public Map
{
public:

    MapGrid() : Map(), m_grid(0) {}
	MapGrid(int16_t lines, int16_t columns);

    virtual ~MapGrid();

	MapGrid* clone() const { return new MapGrid(*this); }

    std::vector<Gentil> gentils() const;
    std::vector<Vilain> vilains() const;
    std::vector<Human> humans() const;
    std::vector<Battle> battles() const;

    void add_gentil(int16_t x, int16_t y, int16_t n) { set_group(x, y, 'G', n); }
    void add_vilain(int16_t x, int16_t y, int16_t n) { set_group(x, y, 'V', n); }
    void add_human(int16_t x, int16_t y, int16_t n) { set_group(x, y, 'H', n); }
    void add_group(Group *group);

	void remove_group(int16_t x, int16_t y) { m_grid[y][x].type = 'E'; }

	int16_t get_number(Point const& point) const { return get_number(point.x(), point.y()); }

	Gentil get_largest_gentil() const;
	Vilain get_largest_vilain() const;

    bool has_battle() const;
    void remove_battles();

    bool is_terminal() const;

	std::string to_string() const;


private:

    /**
     * A structure to represent a square of the map
     */
    struct Square
    {
        char type = 'E';        // square type : 'E' for empty, 'G' for gentil, 'V' for vilain, 'H' for human, 'B' fot battle
        int16_t number;         // number of units in the group (the defenders in case of a battle)

        // Only for battles
        char def;               // defender type : 'G' for gentil, 'V' for vilain, 'H' for human
        char att;               // attacker type : 'G' for gentil, 'V' for vilain
        int16_t number_att;     // number of units in the attacking group
    };

	MapGrid(MapGrid const& map);

    int16_t gentils_number() const;
    int16_t vilains_number() const;

    void result(Move const& move);

    void end_battles();

    char get_type(int16_t x, int16_t y) const { return m_grid[y][x].type; }
    int16_t get_number(int16_t x, int16_t y, bool def = true) const { return def ? m_grid[y][x].number : m_grid[y][x].number_att; }
    void set_number(int16_t x, int16_t y, int16_t n) { n <= 0 ? m_grid[y][x].type = 'E' : m_grid[y][x].number = n; }
    void add_number(int16_t x, int16_t y, int16_t n) { m_grid[y][x].number += n; }
    void set_group(int16_t x, int16_t y, char type, int16_t n);
    void set_battle(int16_t x, int16_t y, char att, int16_t number_att);
    void add_attackers(int16_t x, int16_t y, int16_t n) { m_grid[y][x].number_att += n; }
	char get_att(int16_t x, int16_t y) const { return m_grid[y][x].att; }
	char get_def(int16_t x, int16_t y) const { return m_grid[y][x].def; }

    // A grid of Square to represent the map
    Square **m_grid;
};

#endif // Map_h_INCLUDED
