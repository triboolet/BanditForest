#ifndef TwoVariablesStump_HPP
#define TwoVariablesStump_HPP 

#include <vector>
#include <memory>
#include <utility>
#include "Stump.hpp"

class TwoVariablesStump : public Stump {
  public:
    TwoVariablesStump() {}
    void updatePath(int reward, int action, const std::vector<short> &x_courant);
    void treeBuild(const std::vector<float>& ta, short D, short d, float epsilon);
    void allocPath(const std::vector<uint> &variables, short d);
    std::vector<short> computeYk();
    void free();
    Tree* nextTree(const std::vector<short> &x_courant);
    void freeNextTrees();

    int getVar() const;

  private:
    short var[2];

    std::vector<std::vector<bool>> remainingVariables;
    std::vector<std::vector<std::vector<short>>> y;
    std::vector<std::vector<std::vector<short>>> tk;
    std::vector<std::vector<float>> yp;
    bool isRemaining(uint variable1, uint variable2);
    Tree* next[2][2];
};

#endif 
