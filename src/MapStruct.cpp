
#include "MapStruct.h"

#include <random>
#include <iostream>

using namespace std;

double myPow(double x, int n){
    double res = 1.0;
    double powed = x;
    while(n > 0){
        if(n%2){
            res *= powed;
        }
        powed = powed*powed;
        n /= 2;
    }
    return res;
}

void solveBattle(mapCase* caseToSolve);

std::default_random_engine generator;
bool seeded = false;
static long binomCoeff[256][256];
bool generated = false;


void seedGenerator(){
    if(!seeded){
        generator.seed(10000);
        seeded = true;
    }
    
}
void genrateBinom(){
    if(!generated){
        binomCoeff[0][0] = 1;
        for(int j = 1; j<256; j++){
            binomCoeff[0][j] = 0;
    }
    for(int i = 1; i<256; i++){
        binomCoeff[i][0] = 1;
        for(int j = 1; j<256; j++){
            binomCoeff[i][j] = binomCoeff[i-1][j]+binomCoeff[i-1][j-1];
        }
    }
    generated = true;
    }
    
}

MapStruct::MapStruct(int16_t lines, int16_t columns): Map(lines, columns)
{

    genrateBinom();
    seedGenerator();
    cases = new mapCase[lines*columns];
    for(int i=0; i<lines*columns; i++){
        cases[i].number = 0;
        cases[i].type = ' ';
        cases[i].numberIncoming = 0;
    }
}

MapStruct::MapStruct(const MapStruct& map): Map(map.m_lines, map.m_columns){
    genrateBinom();
    seedGenerator();
    cases = new mapCase[m_lines*m_columns];
    for(int i=0; i<m_lines*m_columns; i++){
        cases[i] = map.cases[i];
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
            if(caseToSolve->type == 'H'){
                std::binomial_distribution<int> convDistribution(caseToSolve->number,P);

                caseToSolve->number = convDistribution(generator);
            }
            else{
                caseToSolve->number = 0;
            }
            
            caseToSolve->number += nDistribution(generator);
            caseToSolve->numberIncoming = 0;
            caseToSolve->type = caseToSolve->incomingType;
        }
        else{
            std::binomial_distribution<int> nDistribution(caseToSolve->number,1-P);
            caseToSolve->number = nDistribution(generator);
            caseToSolve->numberIncoming = 0;
        }
    }
}

double getBinomial(int n, int j,double P){
    return myPow(P,j)*myPow(1-P,n-j)*binomCoeff[n][j];

}

vector<pair<mapCase, double>> sucCases(mapCase* caseToSolve){
    vector<pair<mapCase, double>> cases;
    pair<mapCase, double> p = make_pair(*caseToSolve, 1.0);
    
    if(caseToSolve->numberIncoming == 0 ){
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
        
        if(caseToSolve->type!='H'){
            for(int j = 0; j<=caseToSolve->numberIncoming; j++){
                p.first.number = j;
                p.first.type = caseToSolve->incomingType;
                p.first.numberIncoming = 0;
                p.second = P*getBinomial(caseToSolve->numberIncoming, j, P);
                cases.push_back(p);
            }
            for(int j = 0; j<=caseToSolve->number; j++){
                p.first.number = j;
                p.first.type = caseToSolve->type;
                caseToSolve->numberIncoming = 0;
                p.second = (1-P)*getBinomial(caseToSolve->number, j, (1-P));
                cases.push_back(p);
            }
        }
        else{

            for(int j = 0; j<=caseToSolve->numberIncoming+caseToSolve->number; j++){
                p.first.number = j;
                p.first.type = caseToSolve->incomingType;
                p.first.numberIncoming = 0;
                for(int k = 0; k<=j ; k++){
                    p.second += P*getBinomial(caseToSolve->numberIncoming, j-k, P)*getBinomial(caseToSolve->number, k, P);
                }

                cases.push_back(p);
            }
            for(int j = 0; j<=caseToSolve->number; j++){
                p.first.number = j;
                p.first.type = caseToSolve->type;
                caseToSolve->numberIncoming = 0;
                p.second = (1-P)*getBinomial(caseToSolve->number, j, (1-P));
                cases.push_back(p);
            }
        }
        
        
        
    return cases;
}

vector<pair<MapStruct, double>> MapStruct::successors(){

    
    vector<pair<MapStruct, double>> prev;

    prev.push_back(make_pair(MapStruct(*this), 1.0));

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
    return prev;
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


std::vector<std::pair<MapStruct, double>> MapStruct::successors(Action act){
    MapStruct coucou(*this);
    for (Move const& move : act.moves())
	{
		coucou.result(move);
	}

    return coucou.successors();

}

bool isDeparture(Action& a, int x, int y){
    for(int i = 0; i<a.moves().size(); i++){
        if(a.moves()[i].start().x() == x && a.moves()[i].start().y() == y){
            return true;
        }
    }
    return false;
}
bool isArival(Action& a, int x, int y){
    for(int i = 0; i<a.moves().size(); i++){
        if(a.moves()[i].start().x() == x && a.moves()[i].start().y() == y){
            return true;
        }
    }
    return false;
}

std::vector<Action> actionsOnCase(int x, int y, int n, Action& action){
    
    std::vector<Action> sucActions;
    //Action a;
    sucActions.push_back(action);
    //sucActions.push_back(action);
    std::vector<int> nRemain;
    nRemain.push_back(n);
    for(int dstx = -1; dstx < 2; dstx++){
        for(int dsty = -1; dsty < 2; dsty++){
            if(dsty !=  0 || dsty != 0){
                int nx = x+dstx;
                int ny = y+dsty;
                if(!isDeparture(action, nx, ny)){
                    int size = sucActions.size();
                    for(int l = 0; l<size; l++){
                        int nb = nRemain[l];
                        Action act = sucActions[l];
                        for(int i = 1; i <= nb; i++){
                            
                            //std::cout<<"Coucou"<<std::endl;
                            sucActions.push_back(act);
                            sucActions[sucActions.size()-1].add_move(Move(x,y, i, x+dstx, y+dsty));
                            nRemain.push_back(nb - i);
                        }
                    }
                }
            }
        }
    }
    return sucActions;
}


std::vector<Action> MapStruct::possibleActions(char p){
    std::vector<Action> actions;
    
    actions.push_back(Action());

    unsigned long size = 0;
    for(int i = 0; i<m_lines;  i++){
        for(int j = 0; j<m_columns; j++){
            size = actions.size();
            
            if(cases[i*m_columns+j].type == p){
                
                for(unsigned long l = 0; l<size ;l++){
                    
                    std::vector<Action> newActions = actionsOnCase(j,i, cases[i*m_columns+j].number, actions[l]);
                    for(int m = 1; m<newActions.size(); m++){
                        
                        actions.push_back(newActions[m]);
                    }
                }
            }
            
        }
    }
    return actions;
}