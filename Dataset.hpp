#ifndef DATALOADER_HPP
#define DATALOADER_HPP 

#include <iostream>
#include <vector>
#include <set>

/*
 * A class that reads a file and stores its values in a matrix named contexts
 * and a vector named Ks.
 * The file is supposed to be kind of like a .csv, with each line representing
 * an example, and at the end of it a class (i.e. an arm). The examples (i.e. contexts) are
 * made of binary, and only binary, variables. The separator HAS TO BE
 * tabulations to space the variables, and a single space between the last
 * variable and the class.
 * For example, if the examples  are 10-dimensional, one entry will look like :
 *
 * 0  1   1   1   0   0   0   0   1   0   1 5
 *
 * 5 being the class of this example.
 */
class Dataset {
  public:
    Dataset();
    Dataset(std::string fileName, int variables_num, int lines_num);
    ~Dataset();
    
    /*
     * Reads the file and fills the contexts matrix and Ks vector.
     */
    void loadData(std::string fileName);

    /**
     * Permutes the classes : every class becomes class + permutation % class
     * number
     */
    void permuteClasses(uint permutation);

    // GETTERS

    /*
     * Returns the number of arms (i.e classes)
     */
    int getK() const;

    /*
     * Returns the shape of the contexts matrix.
     */
    std::pair<uint, uint> getShape() const;

    /*
     * Returns the context at position i of the matrix.
     */
    const std::vector<int>& context(uint i);

    /*
     * Returns the value of the jth variable of context i.
     */
    int context(uint i, uint j);

    /*
     * Returns Ks[i], i.e. the class/arm corresponding to context i.
     */
    int best_arm(uint i);

  private:
    /*
     * Matrix of contexts. Each line is an example, each entry is a binary
     * value.
     */
    std::vector<std::vector<int>> contexts;
    /*
     * Vector of the correspond classes/arms.
     */
    std::vector<int> Ks;
    /*
     * Should get rid of this.
     */
    std::set<int> arms;


};

#endif /* DATALOADER_HPP */
