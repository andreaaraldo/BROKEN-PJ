#!/bin/sh
main=./ccnSim
NUM_OF_PROCESSORS=7;

runs=`$main -x General $1 | awk '/runs/{print $4}'`
opp_runall -j$NUM_OF_PROCESSORS $main -u Cmdenv -r 0..$((runs-1)) $1
