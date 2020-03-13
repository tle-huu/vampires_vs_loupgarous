
#include "MapStruct.h"

#include <random>

using namespace std;

void solveBattle(mapCase* caseToSolve);

std::default_random_engine generator;

MapStruct::MapStruct(int16_t lines, int16_t columns): Map(lines, columns)
{


    cases = new mapCase[lines*columns];
    for(int i=0; i<lines*columns; i++){
        cases[i].number = 0;
        cases[i].type = ' ';
        cases[i].numberIncoming = 0;
    }
}

MapStruct::~MapStruct()
{
    delete cases;
}

std::vector<Gentil> MapStruct::gentils() const{
    std::vector<Gentil> gtls;
    for(int i = 0; i<m_lines; i++){
        for(int j = 0; j<m_columns; j++){
            if(cases[i*m_columns+j].type == 'G', cases[i*m_columns+j].number != 0){
                gtls.push_back(Gentil(Point(j,i), cases[i*m_columns+j].number));
            }
        }
    }
    return gtls;
}
 vector<Vilain> MapStruct::vilains() const{
    vector<Vilain> gtls;
    for(int i = 0; i<m_lines; i++){
        for(int j = 0; j<m_columns; j++){
            if(cases[i*m_columns+j].type == 'V', cases[i*m_columns+j].number != 0){
                gtls.push_back(Vilain(Point(j,i), cases[i*m_columns+j].number));
            }
            
        }
    }
    return gtls;

 }
 vector<Human> MapStruct::humans() const{
    vector<Human> gtls;
    for(int i = 0; i<m_lines; i++){
        for(int j = 0; j<m_columns; j++){
            if(cases[i*m_columns+j].type == 'H', cases[i*m_columns+j].number != 0){
                gtls.push_back(Human(Point(j,i), cases[i*m_columns+j].number));
            }
            
        }
    }
    return gtls;
 }
 vector<Battle> MapStruct::battles() const{

 }

 void MapStruct::add_gentil(int16_t x, int16_t y, int16_t n){
     cases[y*m_columns+x].type = 'G';
     cases[y*m_columns+x].number += n;
 }
 void MapStruct::add_vilain(int16_t x, int16_t y, int16_t n){
     cases[y*m_columns+x].type = 'V';
     cases[y*m_columns+x].number += n;
 }
 void MapStruct::add_human(int16_t x, int16_t y, int16_t n){
     cases[y*m_columns+x].type = 'H';
     cases[y*m_columns+x].number += n;
 }
 void MapStruct::add_group(Group *group){
    int x = group->position().x();
    int y = group->position().y();
    cases[y*m_columns+x].type = group->type();
    cases[y*m_columns+x].number += group->number();

 } // not for battle groups

Cell MapStruct::getCell(int x, int y) const{
 Cell c;
 c.element = cases[y*m_columns+x].type;
 c.number = cases[y*m_columns+x].number;
 return c;
}

int16_t MapStruct::heuristic() const{

}
int16_t MapStruct::utility() const{

}

vector<pair<Group*, double> > MapStruct::battle_outcomes(Battle const& battle) const{
    
}

bool MapStruct::has_battle() const{

}
void MapStruct::remove_battles(){

}

 bool MapStruct::is_terminal() const{

 }


bool MapStruct::in_bounds(Point const& point) const{

}


void solveBattle(mapCase* caseToSolve){
    if(caseToSolve->numberIncoming == 0){
        return;
    }
    if(caseToSolve->type == 'H' && caseToSolve->numberIncoming >= caseToSolve->number){
        caseToSolve->type = caseToSolve->incomingType;
        caseToSolve->number += caseToSolve->numberIncoming;
        caseToSolve->numberIncoming = 0;
    }
    else if(caseToSolve->numberIncoming*2 >=caseToSolve->number*3){
        caseToSolve->type = caseToSolve->incomingType;
        caseToSolve->number = caseToSolve->numberIncoming;
        caseToSolve->numberIncoming = 0;
    }
    else if(caseToSolve->numberIncoming*3 <= caseToSolve->number*2){
        caseToSolve->numberIncoming = 0;
    }
    else
    {
        std::uniform_real_distribution<double> disribution(0.0,1.0);

        double P = 0.5;
        if(caseToSolve->numberIncoming < caseToSolve->number){
            P = ((double) caseToSolve->numberIncoming)/(2.0 * (double)caseToSolve->number);
        }
        if(caseToSolve->numberIncoming > caseToSolve->number){
            P = ((double) caseToSolve->numberIncoming)/((double)caseToSolve->number) - 0.5;
        }
        if(disribution(generator)<=P){
            std::binomial_distribution<int> nDistribution(caseToSolve->numberIncoming,P);
            caseToSolve->number = nDistribution(generator);
            caseToSolve->numberIncoming = 0;
            caseToSolve->type = caseToSolve->incomingType;
        }
        else{
            std::binomial_distribution<int> nDistribution(caseToSolve->number,1-P);
            caseToSolve->number = nDistribution(generator);
            caseToSolve->numberIncoming = 0;
            caseToSolve->type = caseToSolve->incomingType;
        }
    }
}

double getBinomial(int n, int j,double P){
    return 1.0;
}

vector<pair<mapCase, double>> sucCases(mapCase* caseToSolve){
    vector<pair<mapCase, double>> cases;
    pair<mapCase, double> p = make_pair(*caseToSolve, 1.0);

    if(caseToSolve->numberIncoming == 0){
        
        cases.push_back(p);
        return cases;
    }

    else if(caseToSolve->type == 'H' && caseToSolve->numberIncoming >= caseToSolve->number){
        p.first.type = caseToSolve->incomingType;
        p.first.number += caseToSolve->numberIncoming;
        p.first.numberIncoming = 0;
        cases.push_back(p);
        return cases;
    }
    else if(caseToSolve->numberIncoming*2 >=caseToSolve->number*3){
        p.first.type = caseToSolve->incomingType;
        p.first.number = caseToSolve->numberIncoming;
        p.first.numberIncoming = 0;
        cases.push_back(p);
        return cases;
    }
    else if(caseToSolve->numberIncoming*3 <= caseToSolve->number*2){
        p.first.numberIncoming = 0;
        cases.push_back(p);
        return cases;
    }

    double P = 0.5;
    if(caseToSolve->numberIncoming < caseToSolve->number){
        P = ((double) caseToSolve->numberIncoming)/(2.0 * (double)caseToSolve->number);
    }
    if(caseToSolve->numberIncoming > caseToSolve->number){
        P = ((double) caseToSolve->numberIncoming)/((double)caseToSolve->number) - 0.5;
    }

        
        for(int j = 0; j<caseToSolve->numberIncoming; j++){
            p.first.number = j;
            p.first.type = caseToSolve->incomingType;
            p.first.numberIncoming = 0;
            p.second = P*getBinomial(caseToSolve->numberIncoming, j, P);
            cases.push_back(p);
        }
        for(int j = 0; j<caseToSolve->number; j++){
            p.first.number = j;
            p.first.type = caseToSolve->incomingType;
            caseToSolve->numberIncoming = 0;
            p.second = (1-P)*getBinomial(caseToSolve->number, j, (1-P));
            cases.push_back(p);
        }
        
        
    return cases;
}

vector<pair<MapStruct, double>> MapStruct::successors(){
    vector<pair<MapStruct, double>> prev;
    unsigned long size = 0;
    for(int i = 0; i<m_lines;  i++){
        for(int j = 0; j<m_columns; j++){
            size = prev.size();
            vector<pair<mapCase, double>> successors = sucCases(&cases[i*m_columns+j]);
            for(unsigned long l = 0; l<size ;l++){
                for(unsigned long k = 1; k<successors.size(); k++){
                    std::pair<MapStruct, double> p = make_pair(MapStruct(prev[l].first), prev[l].second);
                    p.first.cases[i*m_columns+j] = successors[k].first;
                    p.second *= successors[k].second;
                    prev.push_back(p);
                }
                prev[l].first.cases[i*m_columns+j] = successors[0].first;
                prev[l].second *= successors[0].second;
            }
        }
    }
    
}

void MapStruct::end_battles(){
    for(int i = 0; i<m_lines; i++){
        for(int j = 0; j<m_columns; j++){
            solveBattle(&cases[i*m_columns+j]);
        }
    }
}

int16_t MapStruct::gentils_number() const{

}
void MapStruct::result(Move const& move){
    int xStart = move.start().x();
    int yStart = move.start().y();
    int xEnd = move.end().x();
    int yEnd = move.end().y();
    int n = move.number();
	cases[yStart*m_columns+xStart].number -= n;
    if(cases[yStart*m_columns+xStart].type !=
        cases[yEnd*m_columns+xEnd].type &&
        cases[yEnd*m_columns+xEnd].number != 0){
        cases[yEnd*m_columns+xEnd].numberIncoming += n;
        cases[yEnd*m_columns+xEnd].incomingType = cases[yStart*m_columns+xStart].type;
    }
    else
    {
        cases[yEnd*m_columns+xEnd].number += n;
        cases[yEnd*m_columns+xEnd].type = cases[yStart*m_columns+xStart].type;
    }
}
