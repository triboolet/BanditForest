#include <string>
#include <sstream>

inline std::string DATA_FILE;
inline int K;
inline int M; //M
inline int D_MIN; //Dmin
inline int D_MAX; //Dmax
inline int CONTEXTS_NB;
inline int T;
inline int TL;
inline int TIME_DISPLAY;
inline float DELTA;
inline float EPSILON_MIN;
inline float EPSILON_MAX;
inline float C; // c
inline int DRAWS;
inline int NB_TREE;
inline int L; // Number of available variables
inline int NOISE;
inline int VARIABLE_SELECTION_ALG; // Variable Selection algorithm (1 = 1 variable, 2 = 2 variables)
inline float FACTOR_A;
inline float FACTOR_V;

std::string getValueOnLine(std::ifstream& stream);
void load_everything(std::string filename);
