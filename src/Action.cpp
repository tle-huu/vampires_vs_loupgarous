#include "../include/Action.h"

#include <string>
#include <vector>

#include "../include/utils.h"
#include "../include/Point.h"



Action::Action() {}
Action::Action(Move move) { add_move(move); }

Action::Action(std::string actionString){
    std::vector<std::string> subStr = split(actionString, 'x');
    for(int i = 0; i<subStr.size()/5; i++){
        add_move(
            Move(
                stoi(subStr[i]),     //x1
                stoi(subStr[i+1]),   //x2
                stoi(subStr[i+2]),
                stoi(subStr[i+3]),
                stoi(subStr[i+4])
            )
        );
    }
}



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
