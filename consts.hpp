#include <string>
#include <sstream>

inline std::string DATA_FILE;
inline int K;
inline int M; //M
inline int Dmin; //Dmin
inline int Dmax; //Dmax
inline int CONTEXTS_NB;
inline int T;
inline int TL;
inline int TimeDisplay;
inline float delta;
inline float epsilonMin;
inline float epsilonMax;
inline float c; // c
inline int Draws;
inline int nbTree;
inline int L; // Number of available variables
inline int NOISE;
inline float factorA;
inline float factorV;
inline int VARIABLE_SELECTION_ALG; // Variable Selection algorithm (1 = 1 variable, 2 = 2 variables)

std::string getValueOnLine(std::ifstream& stream);
void load_everything(std::string filename);
