#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>

enum State {
 VARIABLE_SELECTION, ACTION_ELIMINATION, EXPLOIT 
};

float RAND_R(float x);
int Card(const std::vector<bool>& Ad);

#endif
