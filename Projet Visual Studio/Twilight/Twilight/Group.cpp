#include "Group.h"

bool operator==(Group const& group1, Group const& group2)
{
	return (group1.position() == group2.position()) && (group1.number() == group2.number());
}

int16_t distance(Group const& group1, Group const& group2)
{
    return distance(group1.position(), group2.position());
}

Gentil& Gentil::operator=(Gentil const& gentil)
{
	if (this != &gentil)
	{
		m_position = gentil.m_position;
		m_number = gentil.m_number;
	}
	return *this;
}

Vilain& Vilain::operator=(Vilain const& vilain)
{
	if (this != &vilain)
	{
		m_position = vilain.m_position;
		m_number = vilain.m_number;
	}
	return *this;
}

int16_t Vilain::min_distance(std::vector<Gentil> const& groups) const
{
	int16_t min_dist = INT16_MAX;
	for (Gentil const& group : groups)
	{
		if (m_number <= group.number())
		{
			int16_t dist = distance(*this, group);
			if (dist < min_dist)
			{
				if (dist == 1)
				{
					return 1;
				}
				min_dist = dist;
			}
		}
	}
	return min_dist;
}

int16_t Human::min_distance(std::vector<Gentil> const& groups) const
{
	int16_t min_dist = INT16_MAX;
	for (Gentil const& group : groups)
	{
		if (m_number <= group.number())
		{
			int16_t dist = distance(*this, group);
			if (dist < min_dist)
			{
				if (dist == 1)
				{
					return 1;
				}
				min_dist = dist;
			}
		}
	}
	return min_dist;
}

int16_t Human::min_distance(std::vector<Vilain> const& groups) const
{
	int16_t min_dist = INT16_MAX;
	for (Vilain const& group : groups)
	{
		if (m_number <= group.number())
		{
			int16_t dist = distance(*this, group);
			if (dist < min_dist)
			{
				if (dist == 1)
				{
					return 1;
				}
				min_dist = dist;
			}
		}
	}
	return min_dist;
}

Point closest_gentil(Gentil const& src, std::vector<Gentil> const& groups)
{
	int16_t min_dist = INT16_MAX;
	Point closest = Point::NULL_POINT;
	for (Gentil const& group : groups)
	{
		int16_t dist = distance(src, group);
		if (dist > 0 && dist < min_dist)
		{
			if (dist == 1)
			{
				return group.position();
			}
			min_dist = dist;
			closest = group.position();
		}
	}
	return closest;
}

Point closest_gentil(Vilain const& src, std::vector<Gentil> const& groups, bool min_number)
{
	int16_t min_dist = INT16_MAX;
	int16_t smallest_number = INT16_MAX;
	Point closest = Point::NULL_POINT;
	for (Gentil const& group : groups)
	{
		if (!min_number || src.number() >= group.number())
		{
			int16_t dist = distance(src, group);
			if (dist < min_dist || (dist == min_dist && group.number() < smallest_number))
			{
				min_dist = dist;
				smallest_number = group.number();
				closest = group.position();
			}
		}
	}
	return closest;
}

Point closest_vilain(Vilain const& src, std::vector<Vilain> const& groups)
{
	int16_t min_dist = INT16_MAX;
	Point closest = Point::NULL_POINT;
	for (Vilain const& group : groups)
	{
		int16_t dist = distance(src, group);
		if (dist > 0 && dist < min_dist)
		{
			if (dist == 1)
			{
				return group.position();
			}
			min_dist = dist;
			closest = group.position();
		}
	}
	return closest;
}

Point closest_vilain(Gentil const& src, std::vector<Vilain> const& groups, bool min_number)
{
	int16_t min_dist = INT16_MAX;
	int16_t smallest_number = INT16_MAX;
	Point closest = Point::NULL_POINT;
	for (Vilain const& group : groups)
	{
		if (!min_number || src.number() >= group.number())
		{
			int16_t dist = distance(src, group);
			if (dist < min_dist || (dist == min_dist && group.number() < smallest_number))
			{
				min_dist = dist;
				smallest_number = group.number();
				closest = group.position();
			}
		}
	}
	return closest;
}

Point closest_human(Gentil const& src, std::vector<Human> const& groups, bool min_number)
{
	int16_t min_dist = INT16_MAX;
	int16_t largest_number = 0;
	Point closest = Point::NULL_POINT;
	for (Human const& group : groups)
	{
		if (!min_number || src.number() >= group.number())
		{
			int16_t dist = distance(src, group);
			if (dist < min_dist || (dist == min_dist && group.number() > largest_number))
			{
				min_dist = dist;
				largest_number = group.number();
				closest = group.position();
			}
		}
	}
	return closest;
}

Point closest_human(Vilain const& src, std::vector<Human> const& groups, bool min_number)
{
	int16_t min_dist = INT16_MAX;
	int16_t largest_number = 0;
	Point closest = Point::NULL_POINT;
	for (Human const& group : groups)
	{
		if (!min_number || src.number() >= group.number())
		{
			int16_t dist = distance(src, group);
			if (dist < min_dist || (dist == min_dist && group.number() > largest_number))
			{
				min_dist = dist;
				largest_number = group.number();
				closest = group.position();
			}
		}
	}
	return closest;
}
