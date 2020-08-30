#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "consts.hpp"


using namespace std;

string getValueOnLine(ifstream& stream) {
  string line;
  string value;
  if (getline(stream, line)) {
    istringstream line_stream(line);
    if (getline(line_stream, line, '=')) {
      getline(line_stream, value);
    }
  }
  return value;
}

void load_everything(string filename) {
  ifstream file;
  file.open(filename);

  DATA_FILE = getValueOnLine(file);
  K = stoi(getValueOnLine(file));
  M = stoi(getValueOnLine(file));
  CONTEXTS_NB = stoi(getValueOnLine(file));
  D_MIN = stoi(getValueOnLine(file)); //Dmin
  D_MAX = stoi(getValueOnLine(file)); //Dmax
  T = stoi(getValueOnLine(file));
  TL = stoi(getValueOnLine(file));
  TIME_DISPLAY = stoi(getValueOnLine(file));
  DELTA = stof(getValueOnLine(file));
  EPSILON_MIN = stof(getValueOnLine(file));
  EPSILON_MAX = stof(getValueOnLine(file));
  C = stof(getValueOnLine(file)); // c
  DRAWS = stoi(getValueOnLine(file));
  NB_TREE = stoi(getValueOnLine(file));
  L = stoi(getValueOnLine(file)); // Number of available variables
  NOISE = stoi(getValueOnLine(file));
  VARIABLE_SELECTION_ALG= stoi(getValueOnLine(file));
  FACTOR_A=4.0*(K/(float)DELTA);
  FACTOR_V=4.0*(K*L/(float)DELTA);

  file.close();
}
