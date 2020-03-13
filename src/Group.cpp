#include "../include/Group.h"

#include <stdint.h> 	/* int16_t */

#include "../include/Point.h"

bool operator==(Group const& group1, Group const& group2)
{
	return (group1.position() == group2.position()) && (group1.number() == group2.number());
}

int16_t distance(Group const& group1, Group const& group2)
{
    return distance(group1.position(), group2.position());
}

std::pair<char, int16_t> Battle::getResult(){
    std::pair<char, int16_t> r;
    if(m_defenders == 'H' && m_number <= m_number_att){
        
        r.first = m_attackers;
        r.second = m_number_att;
    
        
    }else if( 3*m_number <= 2*m_number_att ){
        r.first = m_attackers;
        r.second = m_number_att;
    }
    else{
            double P = 0.0;
            if(m_number_att == m_number){
                P = 0.5;
            }
            else if(m_number_att < m_number){
                P = (double) m_number_att / (2.0*(double) m_number);
            }
            else{
                P = (double) m_number_att / (double) m_number - 0.5;
            }
    }
    return r;
}