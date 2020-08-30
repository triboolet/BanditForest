#include <iostream>
#include <vector>
#include <time.h>
#include <fstream>
#include "Dataset.hpp"
#include "consts.hpp"
#include "BanditForest.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  //marchent pas : 23, 42
  srand(time(NULL));
  string resultsFileName;
  if (argc == 1 || argc > 3) {
    cout << "Enter one and only one data file and one optional results file name" << endl;
    cout << "main [FILE] [RESULTS] ..." << endl;
    exit(EXIT_FAILURE);
  } if (argc == 2) {
    resultsFileName = "results";
  } else {
    resultsFileName = argv[2];
  }
  load_everything(argv[1]);

  Dataset dataset = Dataset(DATA_FILE, M, CONTEXTS_NB);
    
  BanditForest b(dataset);
  b.PlayOLDP3(resultsFileName.c_str());
  
  return 0;
}
