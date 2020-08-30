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

void Tree::ChangeState(State target_state)
// change state 0 -> 1 -> 2
// 0 : Variable Selection
// 1 : Action Selection
// 2 : Exploit 
{
	if (target_state > state ) {
		state=target_state;
    // si on passe de Variable à ActionSelection
		if (target_state == ACTION_ELIMINATION) {
			a=(int)RAND_R((float)K);
			AD=vector<bool>(K, true);
			yk = bestStump->computeYk();
      for (auto stump : stumps) {
        stump->free();
        delete stump;
      }
      bestStump = NULL;
		}
    // Si on passe à Exploit
		if (target_state == EXPLOIT) {
			yk.clear();
      yk.shrink_to_fit();
			AD.clear();
      AD.shrink_to_fit();
      ta.clear();
      ta.shrink_to_fit();
		}
	}
}


void Tree::FreeKMD()
// free the tree
{

  if (d < max_depth) {
    for (auto stump : stumps) {
      stump->freeNextTrees();
    }
    FreePath();
    delete this; 
  } else {
		FreePath();
		delete this;
	}
}

void Tree::FreePath()
// free the path
{
	if (state == VARIABLE_SELECTION) {
    for (auto stump : stumps) {
      stump->free();
      delete stump;
    }
    ta.clear();
    ta.shrink_to_fit();
	}
	if (state == ACTION_ELIMINATION) {
		yk.clear();
    yk.shrink_to_fit();
		AD.clear();
    AD.shrink_to_fit();
    ta.clear();
    ta.shrink_to_fit();
	}
}



void Tree::AllocPath(const std::vector<uint> variables, int d)
// allocate a new path
// p : index de l'action à garder
// q : osef ?
// d : profondeur
{
	this->d=d;
	k=0;
	a=(int)RAND_R((float)K);
	max_depth=(int)(RAND_R((float)(D_MAX - D_MIN)))+D_MIN;
	ta = vector<float>(K, 1.0);
	epsilon = RAND_R((float)(EPSILON_MAX - EPSILON_MIN))+EPSILON_MIN;
	state = VARIABLE_SELECTION;
  bestStump = NULL;
  if (d < max_depth) {
    if (VARIABLE_SELECTION_ALG == 1) {
      stumps.push_back(new OneVariableStump());
    } else if (VARIABLE_SELECTION_ALG == 2) {
      stumps.push_back(new TwoVariablesStump());
    } else {
      stumps.push_back(new OneVariableStump());
      stumps.push_back(new TwoVariablesStump());
    }
    
    for (auto stump : stumps) {
      stump->allocPath(variables, d);
    }
  } else {
    state = ACTION_ELIMINATION;
    AD=vector<bool>(K, true);
    yk = vector<short>(K, 0);
  }
}

Tree* Tree::TreeSearch(const std::vector<short> &x_courant)
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


void Tree::UpdatePath(int y,int k, const std::vector<short> &x_courant)
// Update the counts for variable selection
{
	ta[k]=ta[k]+1;
  for (auto stump : stumps) {
    stump->updatePath(y, k, x_courant);
  }
}

void Tree::UpdateLeaf(int y,int k) {
	ta[k]=ta[k]+1;
	yk[k]=yk[k]+y;
}

void Tree::ActionElimination()
// Action elimination AE
{
	int a;
	int max;
	int b;
  vector<float> alpha(K);

	max=0;b=0;
	for (a=0;a<K;a++) {
		if (AD[a]) {
			if (yk[a]/(float)ta[a] > max/(float)ta[b]) {
				max=yk[a];
				b=a;
			}
			alpha[a]=sqrt(0.5/(float)ta[a]*log((FACTOR_A*(float)(ta[a]*ta[a]))))/C;
		}
	}
  for (a = 0; a < K; a++) {
		if (AD[a] && a!=b) {
			if (max/(float)ta[b] - yk[a]/(float)ta[a] + epsilon>= alpha[a]+alpha[b]) {
				AD[a]=false;
			}
		}
	}
	if (std::count(AD.begin(), AD.end(), true) == 1) {
		this->k=b;
		ChangeState(EXPLOIT);
	}
}

void Tree::TreeBuild()
// Build the tree - variable elimination
{
  for (auto stump : stumps) {
    stump->treeBuild(ta, max_depth, d, epsilon);
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
  a = newAction;
}

// GETTERS
short Tree::getDepth() const {
  return d;
}

short Tree::getMaxDepth() const {
  return max_depth;
}

State Tree::getState() const {
  return state;
}

short Tree::getAction() const {
  return a;
}

vector<bool> Tree::getAD() const {
  return AD;
}

short Tree::getK() const {
  return k;
}

int Tree::LastAction() const
// return the last remaining action in A
{
  auto it = std::find(AD.rbegin(), AD.rend(), true);
  return it == AD.rend() ? 0 : AD.size() - 1 - (it - AD.rbegin());
}

Stump* Tree::getBestStump() const {
  return bestStump;
}
