#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdint.h>     /* (u)intN_t */

#include "Action.h"
#include "State.h"

/**
 * Build a game tree
 * @param initial_state The root state of the tree
 * @param depth The maximum depth of the tree
 */
void build_tree(State &initial_state, uint8_t depth);

/**
 * Find best action in a given state for a player with Minimax algorithm
 * @param state The state at the start of the player's turn
 * @return The action that the player should do
 */
Action minimax(State &state);

/**
 * Calculate the MiniMax of a state where it's the player's turn
 * @param state The state of the game
 * @return The MiniMax value
 */
int16_t max_value(State *state);

/**
 * Calculate the MiniMax of a state where it's the opponent's turn
 * @param state The state of the game
 * @return The MiniMax value
 */
int16_t min_value(State *state);

#endif // MINIMAX_H
