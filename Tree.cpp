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
{
	if (target_state > state ) {
		state=target_state;
    // if we go from VARIABLE_SELECTION to ACTION_ELIMINATION
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
    // if we go from ACTION_ELIMINATION to EXPLOIT
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
{
  // Frees nodes recursively if not terminal
  if (depth < max_depth) {
    for (auto stump : stumps) {
      stump->freeNextTrees();
    }
  }
  freePath();
  delete this;
}

void Tree::freePath()
{
	if (state == VARIABLE_SELECTION) {
    for (auto stump : stumps) {
      stump->free();
      delete stump;
    }
	}
	if (state == ACTION_ELIMINATION) {
		rewardsPerAction.clear();
    rewardsPerAction.shrink_to_fit();
		remainingActions.clear();
    remainingActions.shrink_to_fit();
	}
  drawsPerAction.clear();
  drawsPerAction.shrink_to_fit();
}



void Tree::allocPath(const std::vector<uint> variables, short depth)
// allocate a new path
{
	this->depth=depth;
	bestAction=0;

	lastPlayedAction = (int)RAND_R((float)K);
	max_depth=(int)(RAND_R((float)(D_MAX - D_MIN)))+D_MIN;
	epsilon = RAND_R((float)(EPSILON_MAX - EPSILON_MIN))+EPSILON_MIN;

	drawsPerAction = vector<float>(K, 1.0);
  bestStump = NULL;
	state = VARIABLE_SELECTION;
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

Tree* Tree::treeSearch(const std::vector<short> &current_context)
// Return the path corresponding to the current context
{
  Tree* tree = this;
  Stump* stump = tree->getBestStump();

  // While it's not a terminal node and it is finished selecting variables, we can go
  // deeper
	while (tree->getDepth() < tree->getMaxDepth() && stump != NULL && stump->getVar() != -1){
		tree = stump->nextTree(current_context);
    stump = tree->getBestStump();
	}
	
	return tree;
}


void Tree::updatePath(int reward, int action, const std::vector<short> &current_context)
{
	drawsPerAction[action]=drawsPerAction[action]+1;
  for (auto stump : stumps) {
    stump->updatePath(reward, action, current_context);
  }
}

void Tree::updateLeaf(int reward,int action) {
	drawsPerAction[action] = drawsPerAction[action] + 1;
	rewardsPerAction[action] = rewardsPerAction[action] + reward;
}

void Tree::actionElimination()
{
	int a;
	int max;
	int b;
  vector<float> alpha(K);

	max=0;b=0;
	for (a=0;a<K;a++) {
		if (remainingActions[a]) {
			if (rewardsPerAction[a]/drawsPerAction[a] > max/drawsPerAction[b]) {
				max=rewardsPerAction[a];
				b=a;
			}
			alpha[a]=sqrt(0.5/drawsPerAction[a]*log((FACTOR_A*(drawsPerAction[a]*drawsPerAction[a]))))/C;
		}
	}
  for (a = 0; a < K; a++) {
		if (remainingActions[a] && a!=b) {
			if (max/drawsPerAction[b] - rewardsPerAction[a]/drawsPerAction[a] + epsilon>= alpha[a]+alpha[b]) {
				remainingActions[a]=false;
			}
		}
	}
	if (std::count(remainingActions.begin(), remainingActions.end(), true) == 1) {
		this->bestAction = b;
		changeState(EXPLOIT);
	}
}

void Tree::variableElimination()
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

vector<bool> Tree::getRemainingActions() const {
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
