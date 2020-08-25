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
  Dmin = stoi(getValueOnLine(file)); //Dmin
  Dmax = stoi(getValueOnLine(file)); //Dmax
  T = stoi(getValueOnLine(file));
  TL = stoi(getValueOnLine(file));
  TimeDisplay = stoi(getValueOnLine(file));
  delta = stof(getValueOnLine(file));
  epsilonMin = stof(getValueOnLine(file));
  epsilonMax = stof(getValueOnLine(file));
  c = stof(getValueOnLine(file)); // c
  Draws = stoi(getValueOnLine(file));
  nbTree = stoi(getValueOnLine(file));
  L = stoi(getValueOnLine(file)); // Number of available variables
  NOISE = stoi(getValueOnLine(file));
  VARIABLE_SELECTION_ALG= stoi(getValueOnLine(file));
  factorA=4.0*(K/(float)delta);
  factorV=4.0*(K*L/(float)delta);

  file.close();
}
