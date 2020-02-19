#ifndef GROUP_H
#define GROUP_H

#include <stdint.h> 	/* int16_t */

#include "Point.h"

/**
 * Represents a group of units in the map
 */
class Group
{
public:

    Group(Point position, int16_t number) : m_position(position), m_number(number) {}

    Point position() const { return m_position; }
    void position(Point position) { m_position = position; }
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

    Gentil(Point position, int16_t number) : Group(position, number) {}

    virtual char type() const { return 'G'; }
};

/**
 * Represents a group of opponent's units in the map
 */
class Vilain : public Group
{
public:

    Vilain(Point position, int16_t number) : Group(position, number) {}

    virtual char type() const { return 'V'; }
};

/**
 * Represents a group of human units in the map
 */
class Human : public Group
{
public:

    Human(Point position, int16_t number) : Group(position, number) {}

    virtual char type() const { return 'H'; }
};

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

    virtual char type() const { return 'B'; }


private:

	char m_attackers;		// attacker type : 'G' for gentil, 'V' for vilain
	char m_defenders;		// defender type : 'G' for gentil, 'V' for vilain, 'H' for human
	int16_t m_number_att;
};

#endif // GROUP_H
