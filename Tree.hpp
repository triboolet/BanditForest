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


  void ChangeState(State target_state);
  void FreePath();
  void FreeKMD();
  void AllocPath(const std::vector<uint> variables, int d);
  Tree* TreeSearch(const std::vector<short> &x_courant);
  void UpdatePath(int y,int k, const std::vector<short> &x_courant);
  void UpdateLeaf(int y,int k);
  void ActionElimination();
  void TreeBuild();

  void setAction(short newAction);

  short getDepth() const;
  short getMaxDepth() const;
  State getState() const;
  short getAction() const;
  std::vector<bool> getAD() const;
  short getK() const;
  int LastAction() const;
  Stump* getBestStump() const;

  private:
	// Randomized parameter
  float epsilon;
	short max_depth; // maximum depth

	// variables
	short d; 	// depth
	short k; 	// best action
	short a;	// last played action (for the round-robin)
	State state; // state=0 variable selection, =1 action selection, =2 exploit

  // When state == VariableSelection
  std::vector<Stump*> stumps;
  Stump* bestStump;

	// Counts for state = 1
  std::vector<float> ta; // Number of draws of each action
  std::vector<short> yk;	// Sum of reward of each action
  std::vector<bool> AD;	// set of remaining actions
};

#endif
