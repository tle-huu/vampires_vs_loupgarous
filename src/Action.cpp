#include "../include/Action.h"

#include <stdint.h>     /* (u)intN_t */
#include <string>
#include <vector>

#include "../include/utils.h"
#include "../include/Point.h"

std::string Move::to_string() const
{
	return m_start.to_string() + "x" + int_to_string(m_number) + "x" + m_end.to_string();
}

std::string Action::to_string() const
{
    std::string str = int_to_string(moves_number()) + "x";
    for (Move const& move : m_moves)
    {
        str = str + move.to_string() + "x";
    }
    return str;
}

char* Action::to_cstring() const
{
	return to_c_string(to_string());
}
