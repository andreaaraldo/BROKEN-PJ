#!/bin/sh
main=./ccnSim
NUM_OF_PROCESSORS=6;

runs=`$main -x General $1 | awk '/runs/{print $4}'`
cp ./omnetpp.ini /tmp/ccnsim_output-`date "+%Y-%m-%d_%Hh%M.%S"`.ini
LOGFILE=/tmp/ccnsim_output-`date "+%Y-%m-%d_%Hh%M.%S"`.log
opp_runall -j$NUM_OF_PROCESSORS $main -u Cmdenv -r 0..$((runs-1)) $1 > $LOGFILE

