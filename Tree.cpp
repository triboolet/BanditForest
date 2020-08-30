#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include "Tree.hpp"
#include "OneVariableStump.hpp"
#include "TwoVariablesStump.hpp"
#include "consts.hpp"
#include "utils.hpp"

using namespace std; 

Tree::Tree() {
  
}

Tree::~Tree() {

}

void Tree::changeState(State target_state)
// change state 0 -> 1 -> 2
// 0 : Variable Selection
// 1 : Action Selection
// 2 : Exploit 
{
	if (target_state > state ) {
		state=target_state;
    // si on passe de Variable à ActionSelection
		if (target_state == ACTION_ELIMINATION) {
			lastPlayedAction = (int)RAND_R((float)K);
			remainingActions = vector<bool>(K, true);
			rewardsPerAction = bestStump->computeYk();
      for (auto stump : stumps) {
        stump->free();
        delete stump;
      }
      bestStump = NULL;
		}
    // Si on passe à Exploit
		if (target_state == EXPLOIT) {
			rewardsPerAction.clear();
      rewardsPerAction.shrink_to_fit();
			remainingActions.clear();
      remainingActions.shrink_to_fit();
      drawsPerAction.clear();
      drawsPerAction.shrink_to_fit();
		}
	}
}


void Tree::freeKMD()
// free the tree
{

  if (depth < max_depth) {
    for (auto stump : stumps) {
      stump->freeNextTrees();
    }
    freePath();
    delete this; 
  } else {
		freePath();
		delete this;
	}
}

void Tree::freePath()
// free the path
{
	if (state == VARIABLE_SELECTION) {
    for (auto stump : stumps) {
      stump->free();
      delete stump;
    }
    drawsPerAction.clear();
    drawsPerAction.shrink_to_fit();
	}
	if (state == ACTION_ELIMINATION) {
		rewardsPerAction.clear();
    rewardsPerAction.shrink_to_fit();
		remainingActions.clear();
    remainingActions.shrink_to_fit();
    drawsPerAction.clear();
    drawsPerAction.shrink_to_fit();
	}
}



void Tree::allocPath(const std::vector<uint> variables, int d)
// allocate a new path
{
	this->depth=d;
	bestAction=0;
	lastPlayedAction = (int)RAND_R((float)K);
	max_depth=(int)(RAND_R((float)(D_MAX - D_MIN)))+D_MIN;
	drawsPerAction = vector<float>(K, 1.0);
	epsilon = RAND_R((float)(EPSILON_MAX - EPSILON_MIN))+EPSILON_MIN;
	state = VARIABLE_SELECTION;
  bestStump = NULL;
  if (depth < max_depth) {
    if (VARIABLE_SELECTION_ALG == 1) {
      stumps.push_back(new OneVariableStump());
    } else if (VARIABLE_SELECTION_ALG == 2) {
      stumps.push_back(new TwoVariablesStump());
    } else {
      stumps.push_back(new OneVariableStump());
      stumps.push_back(new TwoVariablesStump());
    }
    
    for (auto stump : stumps) {
      stump->allocPath(variables, depth);
    }
  } else {
    state = ACTION_ELIMINATION;
    remainingActions = vector<bool>(K, true);
    rewardsPerAction = vector<short>(K, 0);
  }
}

Tree* Tree::treeSearch(const std::vector<short> &x_courant)
// Return the path corresponding to the current context
{
  Tree* tree = this;
  Stump* stump = tree->getBestStump();

	while (tree->getDepth() < tree->getMaxDepth() && stump != NULL && stump->getVar() != -1){
		tree = stump->nextTree(x_courant);
    stump = tree->getBestStump();
	}
	
	return tree;
}


void Tree::updatePath(int y,int k, const std::vector<short> &x_courant)
// Update the counts for variable selection
{
	drawsPerAction[k]=drawsPerAction[k]+1;
  for (auto stump : stumps) {
    stump->updatePath(y, k, x_courant);
  }
}

void Tree::updateLeaf(int y,int k) {
	drawsPerAction[k]=drawsPerAction[k]+1;
	rewardsPerAction[k]=rewardsPerAction[k]+y;
}

void Tree::actionElimination()
// Action elimination AE
{
	int a;
	int max;
	int b;
  vector<float> alpha(K);

	max=0;b=0;
	for (a=0;a<K;a++) {
		if (remainingActions[a]) {
			if (rewardsPerAction[a]/(float)drawsPerAction[a] > max/(float)drawsPerAction[b]) {
				max=rewardsPerAction[a];
				b=a;
			}
			alpha[a]=sqrt(0.5/(float)drawsPerAction[a]*log((FACTOR_A*(float)(drawsPerAction[a]*drawsPerAction[a]))))/C;
		}
	}
  for (a = 0; a < K; a++) {
		if (remainingActions[a] && a!=b) {
			if (max/(float)drawsPerAction[b] - rewardsPerAction[a]/(float)drawsPerAction[a] + epsilon>= alpha[a]+alpha[b]) {
				remainingActions[a]=false;
			}
		}
	}
	if (std::count(remainingActions.begin(), remainingActions.end(), true) == 1) {
		this->bestAction = b;
		changeState(EXPLOIT);
	}
}

void Tree::treeBuild()
// Build the tree - variable elimination
{
  for (auto stump : stumps) {
    stump->treeBuild(drawsPerAction, max_depth, depth, epsilon);
    if (stump->getVar() != -1) {
      bestStump = stump;
      for (auto stump : stumps) {
        if (stump != bestStump) {
          stump->free();
          delete stump;
        }
      }
      break;
    }
  }
}

// SETTERS
void Tree::setAction(short newAction) {
  lastPlayedAction = newAction;
}

// GETTERS
short Tree::getDepth() const {
  return depth;
}

short Tree::getMaxDepth() const {
  return max_depth;
}

State Tree::getState() const {
  return state;
}

short Tree::getAction() const {
  return lastPlayedAction;
}

vector<bool> Tree::getAD() const {
  return remainingActions;
}

short Tree::getK() const {
  return bestAction;
}

int Tree::lastAction() const
// return the last remaining action in A
{
  auto it = std::find(remainingActions.rbegin(), remainingActions.rend(), true);
  return it == remainingActions.rend() ? 0 : remainingActions.size() - 1 - (it - remainingActions.rbegin());
}

Stump* Tree::getBestStump() const {
  return bestStump;
}
