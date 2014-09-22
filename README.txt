                  / _____)_	      
  ____ ____ ____ ( (____ (_)_ __ ___  
 / ___) ___)  _ \ \____ \| | '_ ` _ \ 
( (__( (___| | | |_____) | | | | | | |
 \____)____)_| |_(______/|_|_| |_| |_|


Thank you for joining the ccnSim community! This is a brief installation guide. 

You can freely download ccnSim from the project site: http://www.enst.fr/~drossi/ccnSim .

We assume that you have downloaded and installed Omnet++ (version >= 4.1) on your machine. 

You must have a minimal boost installation on your system, before installing ccnSim.

In order to install ccnSim, it is first necessary to patch your omnet installation. Then, you can compile the ccnSim source code. These steps are as follows:

	cd CCNSIM_DIR
	cp ./patch/ctopology.h OMNET_DIR/include/
	cp ./patch/ctopology.cc OMNET_DIR/src/sim
	cd  OMNET_DIR && make && cd CCNSIM
	./scripts/makemake
	make

We suppose that CCNSIM_DIR and OMNET_DIR contain the installation directory of ccnSim  and Omnet++ respectively. 

To run your first simulation
	./ccnSim -u Cmdenv
