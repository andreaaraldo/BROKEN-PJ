#!/bin/sh
main=./ccnSim

runs=`$main -x General $1 | awk '/runs/{print $4}'`
opp_runall -j7 $main -u Cmdenv  -r 0..$((runs-1)) $1
