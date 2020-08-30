#ifndef TwoVariablesStump_HPP
#define TwoVariablesStump_HPP 

#include <vector>
#include <memory>
#include <utility>
#include "Stump.hpp"

class TwoVariablesStump : public Stump {
  public:
    TwoVariablesStump() {}
    void updatePath(int reward, int action, const std::vector<short> &current_context);
    void treeBuild(const std::vector<float>& ta, short max_depth, short depth, float epsilon);
    void allocPath(const std::vector<uint> &variables, short depth);
    std::vector<short> computeRewardsPerAction();
    void free();
    Tree* nextTree(const std::vector<short> &current_context);
    void freeNextTrees();

    int getVar() const;

  private:
    short var[2];

    std::vector<std::vector<bool>> remainingVariables;
    std::vector<std::vector<std::vector<short>>> rewardsPerVariablesPerValue;
    std::vector<std::vector<std::vector<short>>> drawsPerVariablesPerValue;
    std::vector<std::vector<float>> meanRewardPerVariable;
    bool isRemaining(uint variable1, uint variable2);
    Tree* next[2][2];
};

#endif 
