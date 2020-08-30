# BanditForest

A C++ implementation of the [Bandit Forest](https://arxiv.org/pdf/1504.06952.pdf) algorithm from Raphël Féraud, Robin Alleisardo, Tanguy Urvoy and Fabrice Clérot. 

The original paper came with a C implementation. On top of taking advantage of many C++ features, this code adds a new variable selection algorithm (TwoVariablesDecisionStump) and a change-point detector which, once its properly used in coordination of the Bandit Forest algorithm, allows it to adapt when the distribution of actions change, i.e. when the problem is non-stationnary.

## Usage

After cloning the repo, you may compile the code using `make`, which will create an executable called `main`. 
