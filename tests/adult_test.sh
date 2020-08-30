#!/bin/bash

mkdir -p results
# runs the program
./main configs/config_adult_test results/results_adult_test

# test if it works
last_value=`tail -n1 results/results_adult_test.csv | cut -d';' -f9`
[[ $last_value > 0.24 && $last_value < 0.29 ]] && echo -e "\nTest passed !" || echo "ERROR : test not passed ! Value : "$last_value
