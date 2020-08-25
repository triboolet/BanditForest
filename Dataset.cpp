#include "Dataset.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <string>
#include <assert.h>
#include <set>
#include <algorithm>

using namespace std;

Dataset::Dataset() {

}

Dataset::Dataset(std::string fileName, int variables_num, int lines_num) : contexts(lines_num, vector<int>(variables_num)), Ks(lines_num) {
  loadData(fileName);
}

Dataset::~Dataset() {

}


void Dataset::loadData(std::string fileName) {
  ifstream file;
  int variables_num = contexts[0].size();
  int lines_num = Ks.size();
  try {
    file.open(fileName);
    string integer;
    for (int i = 0; i < lines_num; i++) {
      for (int j = 0; j < variables_num; j++) {
        getline(file, integer, '\t');
        contexts[i][j] = stoi(integer);
      } 
      getline(file, integer);
      Ks[i] = stoi(integer);
    }
    
  } catch (const ifstream::failure& ex) {
    cout << "Error opening or reading file " << fileName;
  }
  set<int> unique_classes;
  for (auto i : Ks) {
    unique_classes.emplace(i);
  }
  for (uint i = 0; i < Ks.size(); i++) {
    auto it = find(unique_classes.begin(), unique_classes.end(), Ks[i]);
    int index = distance(unique_classes.begin(), it);
    Ks[i] = index;
  }
}

void Dataset::permuteClasses(uint permutation) {
  set<int> unique_classes;
  for (auto i : Ks) {
    unique_classes.emplace(i);
  }
  for (auto &k : Ks) {
    k =  (k + permutation) % unique_classes.size();
  }
}

std::pair<uint, uint> Dataset::getShape() const {
  return pair<uint, uint>(contexts.size(), contexts[0].size());
}

const std::vector<int>& Dataset::context(uint i) {
  assert(i < contexts.size());
  return contexts[i];
}

int Dataset::context(uint i, uint j) {
  assert(i < contexts.size() && j < contexts[0].size());
  return contexts[i][j];
}

int Dataset::best_arm(uint i) {
  assert(i < Ks.size());
  return Ks[i];
}
