#ifndef Group_h_INCLUDED
#define Group_h_INCLUDED

#include <stdint.h> 	/* int16_t */
#include <vector>

#include "Point.h"

/**
 * Represents a group of units in the map
 */
class Group
{
public:

    Group(Point position = Point::NULL_POINT, int16_t number = 0) : m_position(position), m_number(number) {}

    virtual ~Group() {}

    Point position() const { return m_position; }
    void position(Point position) { m_position = position; }
    int16_t pos_x() const { return m_position.x(); }
    int16_t pos_y() const { return m_position.y(); }
    int16_t number() const { return m_number; }
    void number(int16_t n) { m_number = n; }
    void add_number(int16_t n) { m_number += n; }

    virtual char type() const = 0;


protected:

    Point m_position;
    int16_t m_number;
};

bool operator==(Group const& group1, Group const& group2);

/**
 * Calculate distance between two groups in a map
 * @param group1 The first group
 * @param group2 The second group
 * @return The distance between these two groups
 */
int16_t distance(Group const& group1, Group const& group2);

/**
 * Represents a group of player's units in the map
 */
class Gentil : public Group
{
public:

    Gentil(Point position = Point::NULL_POINT, int16_t number = 0) : Group(position, number) {}

	Gentil& operator=(Gentil const& gentil);

    char type() const { return 'G'; }
};

/**
 * Represents a group of opponent's units in the map
 */
class Vilain : public Group
{
public:

    Vilain(Point position = Point::NULL_POINT, int16_t number = 0) : Group(position, number) {}

	Vilain& operator=(Vilain const& vilain);

    char type() const { return 'V'; }

	int16_t min_distance(std::vector<Gentil> const& groups) const;
};

/**
 * Represents a group of human units in the map
 */
class Human : public Group
{
public:

    Human(Point position = Point::NULL_POINT, int16_t number = 0) : Group(position, number) {}

    char type() const { return 'H'; }

	int16_t min_distance(std::vector<Gentil> const& groups) const;
	int16_t min_distance(std::vector<Vilain> const& groups) const;
};

Point closest_gentil(Gentil const& src, std::vector<Gentil> const& groups);
Point closest_gentil(Vilain const& src, std::vector<Gentil> const& groups, bool min_number = false);

Point closest_vilain(Vilain const& src, std::vector<Vilain> const& groups);
Point closest_vilain(Gentil const& src, std::vector<Vilain> const& groups, bool min_number = false);

Point closest_human(Gentil const& src, std::vector<Human> const& groups, bool min_number = false);
Point closest_human(Vilain const& src, std::vector<Human> const& groups, bool min_number = false);

/**
 * Represents a random battle between two groups in the map
 */
class Battle : public Group
{
public:

    Battle(Point position, char attackers, int16_t number_att, char defenders, int16_t number_def) 
    	: Group(position, number_def), m_attackers(attackers), m_defenders(defenders), m_number_att(number_att) {}

    char attackers() const { return m_attackers; }
    int16_t number_att() const { return m_number_att; }
    void add_attackers(int16_t n) { m_number_att += n; }
    char defenders() const { return m_defenders; }
    int16_t number_def() const { return m_number; }

    char type() const { return 'B'; }


private:

	char m_attackers;		// attacker type : 'G' for gentil, 'V' for vilain
	char m_defenders;		// defender type : 'G' for gentil, 'V' for vilain, 'H' for human
	int16_t m_number_att;
};

#endif // Group_h_INCLUDED
