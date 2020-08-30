#include <vector>
#include <memory>
#include <math.h>
#include <algorithm>
#include <random>
#include "consts.hpp"
#include "utils.hpp"
#include "TwoVariablesStump.hpp"

using namespace std;

void TwoVariablesStump::updatePath(int reward, int action, const std::vector<short> &current_context) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      if (isRemaining(i,j)) {
        short variableValue = current_context[i];
        rewardsPerVariablesPerValue[i][j][action*2 + variableValue] += reward;
        drawsPerVariablesPerValue[i][j][action*2 + variableValue] += 1;
      }
    }
  }
}

void TwoVariablesStump::treeBuild(const std::vector<float>& ta, short D, short d, float epsilon) {
  float maxk, maxp=0.0;
  int amax=0, ip=0, ip2=0;
  vector<vector<float>> alphavar(M, vector<float>(M));
	for (int i=0;i<M;i++) {
    for (int j = 0; j < M; j++) {
      if (isRemaining(i,j)) {
        meanRewardPerVariable[i][j]=0.0;
        // pour chaque v \in {0,1} faire
        for (int q=0;q<2;q++) {
          maxk=0.0;amax=0;
          for (int a=0;a<K;a++) {
            float meanReward = (float)rewardsPerVariablesPerValue[i][j][a*2+q]/(float)drawsPerVariablesPerValue[i][j][a*2+q];
            if (meanReward >= maxk) {
              maxk=meanReward;
              amax=a;
            }
          }
          // Borne de l'équation 5.1
          alphavar[i][j] += sqrt(0.5/ta[amax]*log(FACTOR_V*(D*ta[amax]*ta[amax])))/C;
          meanRewardPerVariable[i][j] += rewardsPerVariablesPerValue[i][j][amax*2+q]/ta[amax];
        }
        if (meanRewardPerVariable[i][j] >= maxp) {
          maxp=meanRewardPerVariable[i][j];
          ip = i;
          ip2 = j;
        }
      }
	  }
  }
	uint NbSon=1;
  // si k = K alors
	for (int i=0;i<M;i++) {
    for (int j = 0; j < M; j++) {
      if (isRemaining(i,j) && (i != ip && j != ip2)) {
        // Retirer de S les variables sous-optimales suivant l'équation (5.1)
        if (maxp - meanRewardPerVariable[i][j] + epsilon >= alphavar[i][j]+alphavar[ip][ip2]) {
          remainingVariables[i][j] = false;
        }
        else NbSon=NbSon+1;
      }
	  }
  }
  // Dans forêt de bandits
  // Si |S| = 1 alors :
	if (NbSon <= 1 && d < D) {
    //printf("yes\n");
		var[0] = ip;
    var[1] = ip2;
    // CreationNoeud(0, c0 + {S, 0})
		for (uint q=0; q<2; q++) {
      for (uint r = 0; r < 2; r++) {
          next[q][r] = new Tree();
          next[q][r]->allocPath({(uint)ip, (uint)ip2}, d+1);
		  }
    }
		free();
	}
}

void TwoVariablesStump::allocPath(const std::vector<uint>& variables, short d) {
  remainingVariables = vector<vector<bool>>(M, vector<bool>(M, true));
  meanRewardPerVariable = vector<vector<float>>(M, vector<float>(M));
  rewardsPerVariablesPerValue = vector<vector<vector<short>>>(M);
  drawsPerVariablesPerValue = vector<vector<vector<short>>>(M);
  var[0] = -1, var[1] = -1;
  if (variables[0] < remainingVariables.size()) {
    remainingVariables[variables[0]][variables[1]] = false;
  }
    
  for (int i=0; i<M; i++) {
    for (int j = 0; j < M; j++) {
      if (remainingVariables[i][j]) {
        if (rewardsPerVariablesPerValue[i].size() == 0) {
          rewardsPerVariablesPerValue[i] = vector<vector<short>>(M);
        }
        rewardsPerVariablesPerValue[i][j] = vector<short>(2*K);
        if (drawsPerVariablesPerValue[i].size() == 0) {
          drawsPerVariablesPerValue[i] = vector<vector<short>>(M);
        }
        drawsPerVariablesPerValue[i][j] = vector<short>(2*K, 1);
      }
    }
  }
}

std::vector<short> TwoVariablesStump::computeRewardsPerAction() {
  auto yk=vector<short>(K);
  for (int i=0;i<M;i++) {
    for (int j = 0; j < M; j++) {
      if (isRemaining(i,j)) {
        for (int s=0;s<2;s++) {
          for (int a=0;a<K;a++) {
            yk[a] += rewardsPerVariablesPerValue[i][j][a*2+s];
          }
        }
      }
    }
  }
  return yk;
}

void TwoVariablesStump::free() {
  for (uint i = 0; i < rewardsPerVariablesPerValue.size(); i++) {
    for (uint j = 0; j < rewardsPerVariablesPerValue[i].size(); j++) {
      rewardsPerVariablesPerValue[i][j].clear();
      rewardsPerVariablesPerValue[i][j].shrink_to_fit();
      drawsPerVariablesPerValue[i][j].clear();
      drawsPerVariablesPerValue[i][j].shrink_to_fit();
    }
    rewardsPerVariablesPerValue[i].clear();
    rewardsPerVariablesPerValue[i].shrink_to_fit();
    drawsPerVariablesPerValue[i].clear();
    drawsPerVariablesPerValue[i].shrink_to_fit();
  }
  rewardsPerVariablesPerValue.clear();
  rewardsPerVariablesPerValue.shrink_to_fit();
  drawsPerVariablesPerValue.clear();
  drawsPerVariablesPerValue.shrink_to_fit();
  for (auto vec : meanRewardPerVariable) {
    vec.clear();
    vec.shrink_to_fit();
  }
  meanRewardPerVariable.clear();
  meanRewardPerVariable.shrink_to_fit();
  for (auto vec : remainingVariables) {
    vec.clear();
    vec.shrink_to_fit();
  }
  remainingVariables.clear();
  remainingVariables.shrink_to_fit();
}

Tree* TwoVariablesStump::nextTree(const std::vector<short> &x_courant) {
  return next[x_courant[var[0]]][x_courant[var[1]]];
}

void TwoVariablesStump::freeNextTrees() {
  if (var[0] != -1) {
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        next[i][j]->freeKMD();
      }
    }
  }
}

int TwoVariablesStump::getVar() const {
  return var[0];
}

// PRIVATE

bool TwoVariablesStump::isRemaining(uint variable1, uint variable2) {
  return remainingVariables[variable1][variable2];
}
