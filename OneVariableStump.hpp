#ifndef ONEVARIABLESTUMP_HPP
#define ONEVARIABLESTUMP_HPP 

#include <vector>
#include <memory>
#include "Stump.hpp"

class OneVariableStump : public Stump {
  public:
    OneVariableStump() {}
    void updatePath(int reward, int action, const std::vector<short> &current_context);
    void treeBuild(const std::vector<float>& ta, short max_depth, short depth, float epsilon);
    void allocPath(const std::vector<uint> &variables, short depth);
    std::vector<short> computeRewardsPerAction();
    void free();
    Tree* nextTree(const std::vector<short> &current_context);
    void freeNextTrees();

    int getVar() const;

  private:
    short var;

    std::vector<bool> remainingVariables;
    std::vector<std::vector<short>> rewardPerVariablePerValue; //y 
    std::vector<std::vector<short>> drawsPerVariablePerValue; // tk
    std::vector<float> meanRewardPerVariable; //yp
    bool isRemaining(uint variable);
    Tree* next[2];
};

#endif 
