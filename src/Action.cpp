#include "Action.h"

#include "utils.h"

std::string Move::to_string() const
{
	return int_to_string(m_number) + "@(" 
		+ int_to_string(m_start.x()) + ","
		+ int_to_string(m_start.y()) + ")->("
		+ int_to_string(m_end.x()) + ","
		+ int_to_string(m_end.y()) + ")";
}

std::ostream& operator<<(std::ostream &out, Move const& move)
{
	out << move.to_string();
	return out;
}

std::string Action::to_string() const
{
    std::string str = "moves: " + int_to_string(m_moves.size());
    for (Move const& move : m_moves)
    {
        str = str + "\n" + move.to_string();
    }
    return str;
}

std::ostream& operator<<(std::ostream &out, Action const& action)
{
	out << action.to_string();
	return out;
}
