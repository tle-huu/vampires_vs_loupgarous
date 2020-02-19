#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdint.h>     /* int16_t */

#include "Action.h"
#include "State.h"

/**
 * Find best action in a given state for a player with Minimax algorithm
 * @param state The state at the start of the player's turn
 * @param max_depth The maximum depth of the game tree to explore
 * @return The action that the player should do
 */
Action minimax(State *state, int16_t max_depth);

/**
 * Calculate the MiniMax of a state where it's the player's turn
 * @param state The state of the game
 * @param depth The depth of the state
 * @return The MiniMax value
 */
int16_t max_value(State *state, int16_t depth);

/**
 * Calculate the MiniMax of a state where it's the opponent's turn
 * @param state The state of the game
 * @param depth The depth of the state
 * @return The MiniMax value
 */
int16_t min_value(State *state, int16_t depth);

/**
 * Find best action in a given state for a player with Minimax algorithm with alpha-beta pruning
 * @param state The state at the start of the player's turn
 * @param max_depth The maximum depth of the game tree to explore
 * @return The action that the player should do
 */
Action minimax_alphabeta(State *state, int16_t max_depth);

/**
 * Calculate the MiniMax of a state where it's the player's turn with alpha-beta pruning
 * @param state The state of the game
 * @param depth The depth of the state
 * @param alpha Alpha value
 * @param beta Beta value
 * @return The MiniMax value
 */
int16_t max_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta);

/**
 * Calculate the MiniMax of a state where it's the opponent's turn with alpha-beta pruning
 * @param state The state of the game
 * @param depth The depth of the state
 * @param alpha Alpha value
 * @param beta Beta value
 * @return The MiniMax value
 */
int16_t min_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta);

/**
 * Find best action in a given state for a player with ExpectMinimax algorithm with alpha-beta pruning
 * @param state The state at the start of the player's turn
 * @param max_depth The maximum depth of the game tree to explore
 * @return The action that the player should do
 */
Action expect_minimax_alphabeta(State *origin, int16_t max_depth);

/**
 * Calculate the ExpectMiniMax of a state with alpha-beta pruning
 * @param state The state of the game
 * @param depth The depth of the state
 * @param alpha Alpha value
 * @param beta Beta value
 * @return The MiniMax value
 */
int16_t expect_minimax_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta);

#endif // MINIMAX_H
