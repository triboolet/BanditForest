#ifndef RBOCPD_INCLUDED
#define RBOCPD_INCLUDED

#include <iostream>
#include <vector>

class RBOCPD {
  public:
    RBOCPD(uint T, uint k);
    ~RBOCPD() {}

    int update(uint t, uint k);

    void setK(uint k);

    // GETTERS
    std::vector<double> getEstimation() const; 

  private:
    uint T;
    double gamma;
    uint like1;
    uint restart;
    uint k;

    std::vector<double> alphas;
    std::vector<double> betas;
    std::vector<double> alphas_betas;
    std::vector<double> forecasterDistribution;
    std::vector<double> pseudoDist;
    std::vector<double> changePointEstimation;

    void updateForecasterDistribution(int reward, double gamma);
    void updateLaplacePrediction(int reward);
};

/*
void updateForecasterDistribution(std::vector<float>& forecasterDistribution, const std::vector<float>& alphas, const std::vector<float>& betas, int reward, float gamma);

void updateLaplacePrediction(std::vector<float>& alphas, std::vector<float>& betas, int reward);

std::vector<float> constructEnvironment(std::vector<float> environment, int period);

std::vector<float> RBOCPD(std::vector<float> environment);
*/

#endif
