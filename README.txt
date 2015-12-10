My secret project

You can freely download ccnSim from the project site: http://www.enst.fr/~drossi/ccnSim .

To install ccnSim, follow the instructions on the manual, Section "Downloading and installing ccnSim" .

To run your first simulation
	./ccnSim -u Cmdenv

At the end, you will find different output files for different values of alpha and different decision policies
	cat results/abilene/F-spr/D-{decision}/R-lru/alpha-{alpha_value}/ccn-id0.sca 

where {alpha_value} can be 0.5, 0.6, 0.7, 0.8, 0.9 or 1 and {decision} can be fix0.1, lcd, lce, prob_cache.

For more details, please refer to the manual
