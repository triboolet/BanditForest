#ifndef STUMP_HPP
#define STUMP_HPP

#include <vector>
#include <memory>
#include "Tree.hpp"

class Stump {
  public:
    Stump() {}
    virtual ~Stump() {}
    virtual void updatePath(int reward, int action, const std::unique_ptr<short[]> &x_courant) = 0;
    virtual void treeBuild(const std::vector<float>& ta, short D, short d, float epsilon) = 0;
    virtual void allocPath(const std::vector<uint> &variables, short d) = 0;
    virtual std::vector<short> computeYk() = 0;
    virtual void free() = 0;
    virtual Tree* nextTree(const std::unique_ptr<short[]> &x_courant) = 0;
    virtual void freeNextTrees() = 0;

    virtual int getVar() const = 0;

  private:
    bool isRemaining(uint variable);
};

#endif 
