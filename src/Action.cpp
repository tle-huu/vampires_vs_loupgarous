#include "Action.h"

#include "utils.h"

std::string Move::to_string() const
{
	return m_start.to_string() + "x" + int_to_string(m_number) + "x" + m_end.to_string();
}

std::string Action::to_string() const
{
    std::string str = int_to_string(m_moves.size()) + "x";
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
