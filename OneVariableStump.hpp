#ifndef ONEVARIABLESTUMP_HPP
#define ONEVARIABLESTUMP_HPP 

#include <vector>
#include <memory>
#include "Stump.hpp"

class OneVariableStump : public Stump {
  public:
    OneVariableStump() {}
    void updatePath(int reward, int action, const std::unique_ptr<short[]> &x_courant);
    void treeBuild(const std::vector<float>& ta, short D, short d, float epsilon);
    void allocPath(const std::vector<uint> &variables, short d);
    std::vector<short> computeYk();
    void free();
    Tree* nextTree(const std::unique_ptr<short[]> &x_courant);
    void freeNextTrees();

    int getVar() const;

  private:
    short var;

    std::vector<bool> remainingVariables;
    std::vector<std::vector<short>> y;
    std::vector<std::vector<short>> tk;
    std::vector<float> yp;
    bool isRemaining(uint variable);
    Tree* next[2];
};

#endif 
