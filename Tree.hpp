#ifndef TREE_HPP
#define TREE_HPP 

#include <iostream>
#include <memory>
#include <vector>

#include "utils.hpp"

class Stump;

/*
 *
 * A decision stump that can either select the best variable when (it's not
 * terminal) or the best action (when it is terminal). All the variable
 * elimination is done in a separate class, called Stump. This one will have one
 * (or multiple) instance(s) of Stump, allowing it to select one or more
 * variables. It will also keep in its attributes variables for the action
 * elimination algorithm.
 *
 */
class Tree {
  
  public:
  Tree();
  ~Tree();


  /*
   * Changes the state of the stump between variable selection 
   * (when it's not a terminal node, and it needs to select the best variable),
   * action selection (when it's a terminal node and it needs to select the best
   * action) and exploit (when it has selected its best action)
   *
   * Parameters:
   *  - target_state : either ACTION_ELIMINATION or EXPLOIT
   */
  void changeState(State target_state);

  /* 
   * Frees the whole tree. Will first free its subtrees (stumps) recursively, 
   * then delete all its allocated variables.
   */
  void freeKMD();

  /*
   * Auxliary function to FreeKMD. Will delete all of the stumps' allocated
   * variables.
   */
  void freePath();


  /*
   * "Creates" the node. Give it a random depth and played action, and allocates
   * every of its variables. If it's a non-terminal node (depth < max_depth),
   * will create One or Two Variable Stumps, will initialize variables regarding
   * the action elimination algorithm if it's a terminal state.
   *
   * Parameters:
   *  - variables : contains the variables selected by the previous node. If the
   *  parent node already visited a variable, this one should not try to select
   *  it. 
   *  - depth : always 1 + the parent node's depth
   */
  void allocPath(const std::vector<uint> variables, short depth);

  /*
   * Will do a tree search based on the values of the context passed in
   * parameter.
   */
  Tree* treeSearch(const std::vector<short> &current_context);

  /*
   * Updates the variable selection variables.
   *
   * Parameters:
   *  - reward : the current reward (binary variable)
   *  - action : the action selected by the algorithm on this iteration
   */
  void updatePath(int reward, int action, const std::vector<short> &current_context);


  /*
   * Updates the action elimination variables.
   *
   * Parameters:
   *  - reward : the current reward (binary variable)
   *  - action : the action selected by the algorithm on this iteration
   */
  void updateLeaf(int reward, int action);

  /*
   * Perform the Action Elimination (AE) algorithm.
   * First computes all the mean rewards per variables, then eliminates
   * variables according to the equation stated in the paper.
   * If there is only one action left, changes the state to EXPLOIT
   */
  void actionElimination();

  /*
   * Go through every of its Stumps to perform the Variable Elimination 
   * (VE) algorithm. Once one stump as selected one or multiple variables, and
   * created children nodes in consequence, delete all other stumps.
   */
  void variableElimination();

  // SETTERS

  /* 
   * Sets the last played action, which will be updated using a round-robin.
   */
  void setAction(short newAction);

  // GETTERS
  short getDepth() const;
  short getMaxDepth() const;
  State getState() const;

  /*
   * Returns the last action played.
   */
  short getAction() const;
  std::vector<bool> getRemainingActions() const;
  short getK() const;

  /*
   * Returns the last remaining Action in remainingActions,
   * i.e. the index of the last value equal to true in remainingActions
   */
  int lastAction() const;
  Stump* getBestStump() const;

  private:
	// Randomized parameters
  float epsilon;
	short max_depth; 

	// variables
	short depth;
	short bestAction;
	short lastPlayedAction;
	State state;

  // When state == VARIABLE_SELECTION
  std::vector<Stump*> stumps;
  Stump* bestStump;

	// Counts for when state == ACTION_ELIMINATION
  std::vector<float> drawsPerAction; // Number of draws of each action
  std::vector<short> rewardsPerAction;	// Sum of reward of each action
  std::vector<bool> remainingActions;	// set of remaining actions
};

#endif
