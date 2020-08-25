#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

#include "RBOCPD.hpp"

using namespace std;

RBOCPD::RBOCPD(uint T, uint k) {
  this->T = T;
  this->k = k;
  like1 = 1;
  gamma = 1.0/static_cast<double>(T);
  forecasterDistribution = vector<double>(1, 1.0);
  pseudoDist = vector<double>(1, 1.0);
  changePointEstimation = vector<double>(1, 1.0);
  //alphas_betas.reserve(2*T);
  //alphas_betas.push_back(1.0);
  //alphas_betas.push_back(1.0);
  alphas.reserve(T);
  alphas.push_back(1.0);
  betas.reserve(T);
  betas.push_back(1.0);
}

int RBOCPD::update(uint t, uint k) {
   int bestExpert = max_element(forecasterDistribution.begin(), forecasterDistribution.end()) 
     - forecasterDistribution.begin();
   int returnVal = 0;
   if (bestExpert > 0) {
     alphas.clear();
     betas.clear();
     forecasterDistribution.clear();
     alphas.push_back(1.0);
     betas.push_back(1.0);
     forecasterDistribution.push_back(1.0);
     restart = t+1;
     like1 = 1;
     returnVal = 1;
   }
   changePointEstimation.push_back(restart+1);
   bool reward = (this->k == k);
   updateForecasterDistribution(reward, gamma);
   updateLaplacePrediction(reward);
   return returnVal;
}

void RBOCPD::setK(uint k) {
  this->k = k;
}


std::vector<double> RBOCPD::getEstimation() const {
  return changePointEstimation;
}

// PRIVATE
void RBOCPD::updateForecasterDistribution(int reward, double gamma) {
  // Likelihood
  vector<double> likelihood;
  likelihood.reserve(alphas.size());
  if (reward == 1) {
    for (uint i = 0; i < alphas.size(); i++) {
      likelihood.push_back(alphas[i]/(alphas[i] + betas[i]));
    }
  } else {
    for (uint i = 0; i < alphas.size(); i++) {
      likelihood.push_back(betas[i]/(alphas[i] + betas[i]));
    }
  }
  /*
  likelihood.reserve(alphas_betas.size());
  if (reward == 1) {
    for (uint i = 0; i < alphas_betas.size()-1; i += 2) {
      likelihood.push_back(alphas_betas[i]/(alphas_betas[i] + alphas_betas[i+1]));
    }
  } else { 
    for (uint i = 1; i < alphas_betas.size(); i += 2) {
      likelihood.push_back(alphas_betas[i]/(alphas_betas[i] + alphas_betas[i-1]));
    }
  }
  */

  double sum = 0.0;
  for (auto &val : pseudoDist) {
    sum += val;
    val *= like1;
  }
  double pseudo_w0 = gamma*like1*sum;
  
  // ForecasterDistribution0
  double forecasterDistribution0 = pseudo_w0;
  // ForecasterDistribution
  for (uint i = 0; i < forecasterDistribution.size(); i++) {
    forecasterDistribution[i] *= (1 - gamma)*likelihood[i];
  }
  forecasterDistribution.push_back(forecasterDistribution0);
  sum = 0.0;
  for (auto& val : forecasterDistribution) {
    sum += val;
  }
  for (auto& val : forecasterDistribution) {
    val /= sum;
  }
  pseudoDist.push_back(pseudo_w0);
  sum = 0.0;
  for (auto& val : pseudoDist) {
    sum += val;
  }
  for (auto& val : pseudoDist) {
    val /= sum;
  }
}


void RBOCPD::updateLaplacePrediction(int reward) {
  for (uint i = 0; i < alphas.size(); i++) {
    alphas[i] += reward;
    betas[i] += (1 - reward);
  }
  alphas.push_back(1);
  betas.push_back(1);
  /*
  for (uint i = 0; i < alphas_betas.size(); i++) {
    alphas_betas[i] += reward*(i % 2 == 0) + (1 - reward)*(i % 2 == 1);
  }
  alphas_betas.push_back(1);
  alphas_betas.push_back(1);
  */
}

/*
vector<float> constructEnvironment(vector<float> environment, int period) {
  vector<float> vect;
  vect.reserve(environment.size());
  for (uint i = 0; i < environment.size(); i++) {
    for (uint j = 0; j < period; j++) {
      vect.push_back(environment[i]);
    }
  }
  return vect;
}



vector<float> RBOCPD(vector<float> environment) {
  uniform_real_distribution<float> distribution(0.0f, 1.0f);
  default_random_engine number_generator;
  uint horizon = environment.size();
  float gamma = 1/horizon;
  vector<float> alphas(1, 1);
  vector<float> betas(1, 1);
  vector<float> forecasterDistribution(1, 1);
  vector<float> changePointEstimation(1, 1);
  for (uint t = 0; t < horizon; t++) {
   int bestExpert = max_element(forecasterDistribution.begin(), forecasterDistribution.end()) 
     - forecasterDistribution.begin();
   changePointEstimation.push_back(bestExpert+1);
   //bool reward = (distribution(number_generator) < environment[t]);
   bool reward = (4 == environment[t]);
   updateForecasterDistribution(forecasterDistribution, alphas, betas, reward, gamma);
   updateLaplacePrediction(alphas, betas, reward);
  }
  return changePointEstimation;
}


void updateForecasterDistribution(vector<float>& forecasterDistribution, const vector<float>& alphas, const vector<float>& betas, int reward, float gamma) {
  // Likelihood
  vector<float> likelihood;
  likelihood.reserve(alphas.size());
  if (reward == 1) {
    for (uint i = 0; i < alphas.size(); i++) {
      likelihood.push_back(alphas[i]/(alphas[i] + betas[i]));
    }
  } else {
    for (uint i = 0; i < alphas.size(); i++) {
      likelihood.push_back(betas[i]/(alphas[i] + betas[i]));
    }
  }
  
  // ForecasterDistribution0
  float forecasterDistribution0;
  for (uint i = 0; i < likelihood.size(); i++) {
    forecasterDistribution0 += gamma * likelihood[i] * forecasterDistribution[i];
  }

  // ForecasterDistribution
  for (uint i = 0; i < forecasterDistribution.size(); i++) {
    forecasterDistribution[i] *= (1 - gamma)*likelihood[i];
  }
  forecasterDistribution.push_back(forecasterDistribution0);
  float sum = 0;
  for (float &val : forecasterDistribution) {
    sum += val;
  }
  for (float &forecaster : forecasterDistribution) {
    forecaster /= sum;
  }
}


void updateLaplacePrediction(vector<float>& alphas, vector<float>& betas, int reward) {
  for (uint i = 0; i < alphas.size(); i++) {
    alphas[i] += reward;
    betas[i] += (1 - reward);
  }
  alphas.push_back(1);
  betas.push_back(1);
}

vector<float> constructEnvironment(vector<float> environment, int period) {
  vector<float> vect;
  vect.reserve(environment.size());
  for (uint i = 0; i < environment.size(); i++) {
    for (uint j = 0; j < period; j++) {
      vect.push_back(environment[i]);
    }
  }
  return vect;
}
*/
