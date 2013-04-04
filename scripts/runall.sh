#!/bin/sh
main=./ccnSim

runs=`$main -x General | awk '/runs/{print $4}'`
opp_runall -j2 $main -u Cmdenv -r 0..$((runs-1))
