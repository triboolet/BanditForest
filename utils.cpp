#include "utils.hpp"
#include "consts.hpp"

float RAND_R(float x) {
  return (float)rand()/(double)(RAND_MAX-1.0)*x;
}
