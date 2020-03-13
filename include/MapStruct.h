#ifndef MAP_STRUCT_HEADER
#define MAP_STRUCT_HEADER

#include "Map.h"

struct mapCase{
    char type;
    unsigned char number;
    char incomingType;
    unsigned char numberIncoming;
};

class MapStruct : public Map
{
private:
    mapCase* cases;
    
public:
    MapStruct(int16_t lines = 0, int16_t columns = 0);
    ~MapStruct();

    virtual std::vector<Gentil> gentils() const;
    virtual std::vector<Vilain> vilains() const;
    virtual std::vector<Human> humans() const;
    virtual std::vector<Battle> battles() const;

    virtual void add_gentil(int16_t x, int16_t y, int16_t n);
    virtual void add_vilain(int16_t x, int16_t y, int16_t n);
    virtual void add_human(int16_t x, int16_t y, int16_t n);
    virtual void add_group(Group *group); // not for battle groups

    virtual Cell getCell(int x, int y) const;

    int16_t heuristic() const;
    int16_t utility() const;

    std::vector<std::pair<Group*, double> > battle_outcomes(Battle const& battle) const;

    virtual bool has_battle() const;
    virtual void remove_battles();

    virtual bool is_terminal() const;

    //void result(Action const& action);

    bool in_bounds(Point const& point) const;

    std::vector<std::pair<MapStruct, double>> successors();

    

protected:
    virtual void end_battles();

    virtual int16_t gentils_number() const;
    virtual void result(Move const& move);

};

std::vector<std::pair<mapCase, double>> sucCases(mapCase* caseToSolve);



#endif