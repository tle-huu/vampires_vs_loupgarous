#ifndef Minimax_h_INCLUDED
#define Minimax_h_INCLUDED

#include <stdint.h>     /* int16_t */

#include "Action.h"
#include "Map.h"
#include "State.h"
#include "Chrono.h"

// Maximum time (in seconds) before get_best_action method needs to end
static constexpr double MAX_TIME = 1.904081995;

// Method to generate actions
static constexpr int DEFAULT_ACTION_METHOD = 4;

// Maximum number of friendly groups
static constexpr int16_t DEFAULT_MAX_GROUPS = 2;

/**
 * Calculate the best action to do in a given state
 * @param map The current state of the game
 * @param max_depth The maximum depth of the game tree to explore
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The best action found
 */
Action get_best_action(Map *map, int16_t max_depth, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Find best action in a given state for a player with Minimax algorithm
 * @param state The state at the start of the player's turn
 * @param max_depth The maximum depth of the game tree to explore
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The action that the player should do
 */
Action minimax(State *state, int16_t max_depth, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Calculate the MiniMax of a state where it's the player's turn
 * @param state The state of the game
 * @param depth The depth of the state
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The MiniMax value
 */
int16_t max_value(State *state, int16_t depth, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Calculate the MiniMax of a state where it's the opponent's turn
 * @param state The state of the game
 * @param depth The depth of the state
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The MiniMax value
 */
int16_t min_value(State *state, int16_t depth, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Find best action in a given state for a player with Minimax algorithm with alpha-beta pruning
 * @param state The state at the start of the player's turn
 * @param max_depth The maximum depth of the game tree to explore
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The action that the player should do
 */
Action minimax_alphabeta(State *state, int16_t max_depth, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Calculate the MiniMax of a state where it's the player's turn with alpha-beta pruning
 * @param state The state of the game
 * @param depth The depth of the state
 * @param alpha Alpha value
 * @param beta Beta value
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The MiniMax value
 */
int16_t max_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Calculate the MiniMax of a state where it's the opponent's turn with alpha-beta pruning
 * @param state The state of the game
 * @param depth The depth of the state
 * @param alpha Alpha value
 * @param beta Beta value
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The MiniMax value
 */
int16_t min_value_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Find best action in a given state for a player with ExpectMinimax algorithm with alpha-beta pruning
 * @param state The state at the start of the player's turn
 * @param max_depth The maximum depth of the game tree to explore
 * @param chrono The chrono measuring the time spent since the beginning of the algorithm
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The action that the player should do
 */
Action expect_minimax_alphabeta(State *origin, int16_t max_depth, Chrono const& chrono, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

/**
 * Calculate the ExpectMiniMax of a state with alpha-beta pruning
 * @param state The state of the game
 * @param depth The depth of the state
 * @param alpha Alpha value
 * @param beta Beta value
 * @param chrono The chrono measuring the time spent since the beginning of the algorithm
 * @param action_method The method to generate actions
 * @param max_gentil_groups The maximum number of gentil groups
 * @param max_vilain_groups The maximum number of vilain groups
 * @return The MiniMax value
 */
int16_t expect_minimax_alphabeta(State *state, int16_t depth, int16_t alpha, int16_t beta, Chrono const& chrono, int action_method = DEFAULT_ACTION_METHOD, 
	int16_t max_gentil_groups = DEFAULT_MAX_GROUPS, int16_t max_vilain_groups = DEFAULT_MAX_GROUPS);

#endif // Minimax_h_INCLUDED
