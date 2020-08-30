#include <vector>
#include <memory>
#include <math.h>
#include <algorithm>
#include <random>
#include <valarray>
#include "consts.hpp"
#include "utils.hpp"
#include "OneVariableStump.hpp"

using namespace std;


void OneVariableStump::updatePath(int reward, int action, const std::vector<short> &x_courant) {
  for (uint i = 0; i < remainingVariables.size(); i++) {
    if (isRemaining(i)) {
      short variableValue = x_courant[i];
      y[i][action*2 + variableValue] += reward;
      tk[i][action*2 + variableValue] += 1;
    }
  }
}

void OneVariableStump::treeBuild(const std::vector<float>& ta, short D, short d, float epsilon) {
  float maxk, maxp=0.0;
  int amax=0, ip=0;
  vector<float> alphavar(M);
  valarray<float> mus(K*2);
  
  for (uint i = 0; i < y.size(); i++) {
    if (isRemaining(i)) {
      yp[i] = 0.0;
      transform(y[i].begin(), y[i].end(), tk[i].begin(), begin(mus), std::divides<float>());
      valarray<float> evenMus = mus[slice(0, mus.size()/2, 2)];
      valarray<float> oddMus = mus[slice(1, mus.size()/2, 2)];
      auto max_it_even = max_element(begin(evenMus), end(evenMus)), max_it_odd = max_element(begin(oddMus), end(oddMus));
      int amax_even = max_it_even - begin(evenMus), amax_odd = max_it_odd - begin(oddMus);
      alphavar[i] += (
          sqrt(0.5/ta[amax_even]
            *log(FACTOR_V
              *(D*ta[amax_even]
                *ta[amax_even])))
          /C);
      alphavar[i] += (
          sqrt(0.5/ta[amax_odd]
            *log(FACTOR_V
              *(D*ta[amax_odd]
                *ta[amax_odd])))
          /C);
      yp[i] += y[i][amax_even*2+0]/ta[amax_even] + y[i][amax_odd*2+1]/ta[amax_odd];
      if (yp[i] >= maxp) {
        maxp=yp[i];
        ip=i;
      }
    }
  }
  /*
	for (int i=0;i<M;i++) {
		if (isRemaining(i)) {
			yp[i]=0.0;
      transform(y[i].begin(), y[i].end(), tk[i].begin(), begin(mus), std::divides<float>());
      valarray<float> evenMus = mus[slice(0, mus.size()/2, 2)];
      valarray<float> oddMus = mus[slice(1, mus.size()/2, 2)];
      auto max_it_even = max_element(begin(evenMus), end(evenMus)), max_it_odd = max_element(begin(oddMus), end(oddMus));
      int amax_even = max_it_even - begin(evenMus), amax_odd = max_it_odd - begin(oddMus);
      // pour chaque v \in {0,1} faire
			for (int q=0;q<2;q++) {
				maxk=0.0;amax=0;
				for (int a=0;a<K;a++) {
          float meanReward = (float)y[i][a*2+q]/(float)tk[i][a*2+q];
					if (meanReward >= maxk) {
						maxk=meanReward;
       			amax=a;
					}
				}
        if (q == 0) {
          cout << amax << "," << amax_even << endl;
        }
        if (q == 1) {
          cout << amax << "," << amax_odd << endl;
        }
        // Borne de l'équation 5.1
				alphavar[i] += sqrt(0.5/(float)ta[amax]*log(factorV*(float)(D*ta[amax]*ta[amax])))/c;
        // yp = \mu_k_v_i ou \mu_i, bref le mean reward pour la variable i
				yp[i] += (float)y[i][amax*2+q]/(float)ta[amax];
        //yp[i]=yp[i] + maxk;
			}
      if (yp[i] >= maxp) {
        maxp=yp[i];
        ip=i;
      }
		}
	}
  */
	uint NbSon=1;
  // si k = K alors
	for (int i=0;i<M;i++) {
		if (isRemaining(i) && i!=ip) {
      // Retirer de S les variables sous-optimales suivant l'équation (5.1)
			if (maxp - yp[i]+epsilon >= alphavar[i]+alphavar[ip]) {
				remainingVariables[i] = false;
			}
			else { 
        NbSon=NbSon+1;
		  }
	  }
  }
  // Dans forêt de bandits
  // Si |S| = 1 alors :
	if (NbSon <= 1 && d < D) {
		var=ip;
    // CreationNoeud(0, c0 + {S, 0})
		for (int q=0; q<2; q++) {
        next[q] = new Tree();
				next[q]->allocPath({(uint)ip}, d+1);
		}
		free();
	}
}

void OneVariableStump::allocPath(const std::vector<uint>& variables, short d) {
  var = -1;
  remainingVariables = vector<bool>(M, true);
  yp = vector<float>(M);
  y = vector<vector<short>>(M);
  tk = vector<vector<short>>(M);
  int nbVar=M-d;
  uint exceedingVarsNb = nbVar - L;
  if (exceedingVarsNb > 0) {
    replace(remainingVariables.begin(), remainingVariables.begin()+exceedingVarsNb, true, false);
    shuffle(remainingVariables.begin(), remainingVariables.end(), default_random_engine());
  }
  if (variables[0] < remainingVariables.size()) {
    remainingVariables[variables[0]] = false;
  }
    
  for (int i=0; i<M; i++) {
    if (isRemaining(i)) {
      y[i] = vector<short>(2*K);
      tk[i] = vector<short>(2*K, 1);
    }
  }
}

std::vector<short> OneVariableStump::computeYk() {
  auto yk=vector<short>(K);
  for (int i=0;i<M;i++) {
     if (y[i].size() > 0) {
      for (int s=0;s<2;s++) {
          for (int a=0;a<K;a++) {
            yk[a] += y[i][a*2+s];
          }
       }
     }
  }
  return yk;
}

void OneVariableStump::free() {
  for (uint i = 0; i < y.size(); i++) {
    y[i].clear();
    tk[i].clear();
  }
  y.clear();
  tk.clear();
  yp.clear();
  remainingVariables.clear();
}

Tree* OneVariableStump::nextTree(const std::vector<short> &x_courant) {
  return next[x_courant[var]];
}

void OneVariableStump::freeNextTrees() {
  if (var != -1) {
    for (int i = 0; i < 2; i++) {
      next[i]->freeKMD();
    }
  }
}

int OneVariableStump::getVar() const {
  return var;
}

// PRIVATE

bool OneVariableStump::isRemaining(uint variable) {
  return remainingVariables[variable];
}
