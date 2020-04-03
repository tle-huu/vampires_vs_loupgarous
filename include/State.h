#ifndef State_h_INCLUDED
#define State_h_INCLUDED

#include <stdint.h>     /* int16_t */
#include <vector>

#include "Map.h"
#include "Action.h"

// Maximum number of friendly groups
static constexpr int16_t MAX_GROUPS = 3;

/**
 * Represents a state of the game
 */
class State
{
public:

	State(Map *map = 0, bool turn = true, bool chance = false, double proba = 1.0) : m_map(map), m_turn(turn), m_chance(chance), m_proba(proba) {}
	State(State &&state);

	virtual ~State() { delete m_map; }

	State& operator=(State const& state);
    
	Map* map() { return m_map; }

    bool is_max() const { return m_turn; }
    bool is_chance() const { return m_chance; }

    double proba() const { return m_proba; }

    int16_t utility() const { return m_map->utility(m_turn); }

    bool is_terminal() const { return m_map->is_terminal(); }

    std::vector<Action> actions() const;

    State result(Action const& action) const;

    std::vector<State> successors() const;

	bool preferable_than(State *state) const { return m_map->preferable_than(state->m_map); }


private:

    Map *m_map;
    bool m_turn;		// true for gentil's turn, false for vilain's turn
    bool m_chance;		// true if it's a state with random
    double m_proba;
};

#endif // State_h_INCLUDED
