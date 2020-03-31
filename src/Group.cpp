#include "Group.h"

bool operator==(Group const& group1, Group const& group2)
{
	return (group1.position() == group2.position()) && (group1.number() == group2.number());
}

int16_t distance(Group const& group1, Group const& group2)
{
    return distance(group1.position(), group2.position());
}
