#ifndef TREE_HPP
#define TREE_HPP 

#include <iostream>
#include <memory>
#include <vector>

#include "utils.hpp"

class Stump;

class Tree {
  
  public:
  Tree();
  ~Tree();


  void changeState(State target_state);
  void freePath();
  void freeKMD();
  void allocPath(const std::vector<uint> variables, int d);
  Tree* treeSearch(const std::vector<short> &x_courant);
  void updatePath(int y,int k, const std::vector<short> &x_courant);
  void updateLeaf(int y,int k);
  void actionElimination();
  void treeBuild();

  void setAction(short newAction);

  short getDepth() const;
  short getMaxDepth() const;
  State getState() const;
  short getAction() const;
  std::vector<bool> getAD() const;
  short getK() const;
  int lastAction() const;
  Stump* getBestStump() const;

  private:
	// Randomized parameter
  float epsilon;
	short max_depth; // maximum depth

	// variables
	short depth; 	// depth
	short bestAction; 	// best action
	short lastPlayedAction;	// last played action (for the round-robin)
	State state; // state=0 variable selection, =1 action selection, =2 exploit

  // When state == VariableSelection
  std::vector<Stump*> stumps;
  Stump* bestStump;

	// Counts for state = 1
  std::vector<float> drawsPerAction; // Number of draws of each action
  std::vector<short> rewardsPerAction;	// Sum of reward of each action
  std::vector<bool> remainingActions;	// set of remaining actions
};

#endif
