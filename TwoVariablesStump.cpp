#include <vector>
#include <memory>
#include <math.h>
#include <algorithm>
#include <random>
#include "consts.hpp"
#include "utils.hpp"
#include "TwoVariablesStump.hpp"

using namespace std;

void TwoVariablesStump::updatePath(int reward, int action, const std::vector<short> &x_courant) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      if (isRemaining(i,j)) {
        short variableValue = x_courant[i];
        y[i][j][action*2 + variableValue] += reward;
        tk[i][j][action*2 + variableValue] += 1;
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
        yp[i][j]=0.0;
        // pour chaque v \in {0,1} faire
        for (int q=0;q<2;q++) {
          maxk=0.0;amax=0;
          for (int a=0;a<K;a++) {
            float meanReward = (float)y[i][j][a*2+q]/(float)tk[i][j][a*2+q];
            if (meanReward >= maxk) {
              maxk=meanReward;
              amax=a;
            }
          }
          // Borne de l'équation 5.1
          alphavar[i][j] += sqrt(0.5/ta[amax]*log(FACTOR_V*(D*ta[amax]*ta[amax])))/C;
          // yp = \mu_k_v_i ou \mu_i, bref le mean reward pour la variable i
          yp[i][j] += y[i][j][amax*2+q]/ta[amax];
          //yp[i]=yp[i] + maxk;
        }
        if (yp[i][j] >= maxp) {
          maxp=yp[i][j];
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
        if (maxp - yp[i][j] + epsilon >= alphavar[i][j]+alphavar[ip][ip2]) {
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
          next[q][r]->AllocPath({(uint)ip, (uint)ip2}, d+1);
		  }
    }
		free();
	}
}

void TwoVariablesStump::allocPath(const std::vector<uint>& variables, short d) {
  remainingVariables = vector<vector<bool>>(M, vector<bool>(M, true));
  yp = vector<vector<float>>(M, vector<float>(M));
  y = vector<vector<vector<short>>>(M);
  tk = vector<vector<vector<short>>>(M);
  var[0] = -1, var[1] = -1;
  if (variables[0] < remainingVariables.size()) {
    remainingVariables[variables[0]][variables[1]] = false;
  }
    
  for (int i=0; i<M; i++) {
    for (int j = 0; j < M; j++) {
      if (remainingVariables[i][j]) {
        if (y[i].size() == 0) {
          y[i] = vector<vector<short>>(M);
        }
        y[i][j] = vector<short>(2*K);
        if (tk[i].size() == 0) {
          tk[i] = vector<vector<short>>(M);
        }
        tk[i][j] = vector<short>(2*K, 1);
      }
    }
  }
}

std::vector<short> TwoVariablesStump::computeYk() {
  auto yk=vector<short>(K);
  for (int i=0;i<M;i++) {
    for (int j = 0; j < M; j++) {
      if (isRemaining(i,j)) {
        for (int s=0;s<2;s++) {
          for (int a=0;a<K;a++) {
            yk[a] += y[i][j][a*2+s];
          }
        }
      }
    }
  }
  return yk;
}

void TwoVariablesStump::free() {
  for (uint i = 0; i < y.size(); i++) {
    for (uint j = 0; j < y[i].size(); j++) {
      y[i][j].clear();
      y[i][j].shrink_to_fit();
      tk[i][j].clear();
      tk[i][j].shrink_to_fit();
    }
    y[i].clear();
    y[i].shrink_to_fit();
    tk[i].clear();
    tk[i].shrink_to_fit();
  }
  y.clear();
  y.shrink_to_fit();
  tk.clear();
  tk.shrink_to_fit();
  for (auto vec : yp) {
    vec.clear();
    vec.shrink_to_fit();
  }
  yp.clear();
  yp.shrink_to_fit();
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
        next[i][j]->FreeKMD();
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
