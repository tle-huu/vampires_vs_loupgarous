#include "Group.h"

bool operator==(Group const& group1, Group const& group2)
{
	return (group1.position() == group2.position()) && (group1.number() == group2.number());
}

int16_t distance(Group const& group1, Group const& group2)
{
    return distance(group1.position(), group2.position());
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
