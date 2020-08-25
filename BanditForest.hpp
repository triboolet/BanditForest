#ifndef BANDITFOREST_HPP
#define BANDITFOREST_HPP 

#include <memory>
#include <vector>
#include <unordered_map>

#include "Dataset.hpp"
#include "Tree.hpp"
#include "RBOCPD.hpp"

class BanditForest {
  public:
  BanditForest(Dataset d);
  ~BanditForest();

  void SaveResults_Gain(const char* nomfichier);
  int RoundRobin(int k, const std::vector<bool>& A);
  void PrintContext();
  void GetContext(uint pos, uint bruit);
  int Decide_Vote(Tree **p);
  int PlayOLDP3(const char *nomfichier);

  private:
  std::unique_ptr<short[]> x_courant;
  Dataset x;
  uint pos_current;
  int a_current;
  Tree** Forest;

  std::unordered_map<Tree*, RBOCPD*> changePointDetectors;

};

#endif /* BANDITFOREST_HPP */
