#include "utils.hpp"
#include "consts.hpp"

float RAND_R(float x) {
  return (float)rand()/(double)(RAND_MAX-1.0)*x;
}

int Card(const std::vector<bool>& AD)
// return the cardinal of AD[i]=1
{
	int i,s;
	
	s=0;	
	for (i=0;i<K;i++) {
		if (AD[i]) s++;
	}	
	return s;
}
