#!/bin/bash

mkdir -p results
# runs the program
./main configs/config_forest_test results/results_forest_test

# test if it works
last_value=`tail -n1 results/results_forest_test.csv | cut -d';' -f9`
[[ $last_value > 0.62 ]] && echo -e "\nTest passed !" || echo "ERROR : test not passed ! Value : "$last_value
