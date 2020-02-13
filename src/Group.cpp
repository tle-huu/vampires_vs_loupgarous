#include "../include/Group.h"

#include <stdint.h> 	/* (u)intN_t */

#include "../include/Point.h"

uint16_t distance(Group const& group1, Group const& group2)
{
    return distance(group1.position(), group2.position());
}
