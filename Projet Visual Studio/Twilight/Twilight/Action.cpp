#include "Action.h"

#include "utils.h"

std::string Move::to_string() const
{
	return int_to_string(m_number) + "@" + m_start.to_string() + "->" + m_end.to_string();
}

bool operator==(Move const& move1, Move const& move2)
{
	return (move1.start() == move2.start() && move1.end() == move2.end() && move1.number() == move2.number());
}

std::ostream& operator<<(std::ostream &out, Move const& move)
{
	out << move.to_string();
	return out;
}

void Action::add_moves(std::vector<Move> moves)
{
	for (Move const& move : moves)
	{
		add_move(move);
	}
}

bool Action::valid() const
{
	bool res = !empty();
	std::vector<Point> points;
	for (Move const& move : m_moves)
	{
		points.push_back(move.start());
	}
	for (Move const& move : m_moves)
	{
		if (find_element(points, move.end()))
		{
			return false;
		}
	}
	return res;
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
