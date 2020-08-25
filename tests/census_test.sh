#!/bin/bash

# runs the program
./main configs/config_census_test results/results_census_test

# test if it works
mkdir -p results
last_value=`tail -n1 results/results_census_test.csv | cut -d';' -f9`
[[ $last_value > 0.37 ]] && echo -e "\nTest passed !" || echo "ERROR : test not passed ! Value : "$last_value
