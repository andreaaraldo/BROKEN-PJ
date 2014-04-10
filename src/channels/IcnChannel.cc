//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

//<aa>
#include "IcnChannel.h"
#include "error_handling.h"
#include "cmodule.h"
#include "ccnsim.h" // for CCN_D and systemIsStable
#include "ccn_data.h"

Define_Channel(IcnChannel);

void IcnChannel::initialize()
{
        cDatarateChannel::initialize();
        double price_asc = par("price_asc");
        double price_desc = par("price_desc");

        #ifdef SEVERE_DEBUG
		cTopology topo;
		topo.extractByNedTypeName(cStringTokenizer("modules.statistics.statistics").asVector() );
        std::stringstream ermsg; 
	    int num_nodes = topo.getNumNodes();
		if (num_nodes!=1) {
			ermsg<<"Number of statistics nodes: "<<num_nodes;
		    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	    #endif
        
        //cGate* sourceGate = getSourceGate();
        //cModule* node_ = sourceGate->getOwnerModule();
        //int source_index = node_->getIndex();
        
        int source_index = getSourceGate()->getOwnerModule()->getIndex();
        int dest_index = getSourceGate()->getNextGate()->getOwnerModule()->getIndex();
        price = source_index < dest_index ? price_asc : price_desc;
        #ifdef SEVERE_DEBUG
        std::stringstream msg; 

		ermsg.str(""); 
		msg<<"price for each data pkt from node["<<source_index<<"] to node["
			<<dest_index<<"]: "<< price;
	    debug_message(__FILE__,__LINE__,msg.str().c_str() );
	    #endif

		// Registering this IcnChannel to the statistics module
		vector<string> name_vec(1,"modules.statistics.statistics");
		topo.extractByNedTypeName(name_vec );
		#ifdef SEVERE_DEBUG
		if (topo.getNumNodes() != 1) {
		    std::stringstream ermsg; 
			ermsg<<topo.getNumNodes()<<" modules have been found whereas there must be only "<<
					"one statistics module: ";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif
		statistics_object = (statistics*) topo.getNode(0)->getModule();
		statistics_object->registerIcnChannel(this);
		clear_stat();
}


void IcnChannel::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
        cDatarateChannel::processMessage(msg,t,result);
        if( msg->getKind() == CCN_D)
		{
		    count++;
			ccn_data* data_msg = (ccn_data*) msg;
			data_msg->setCost(price);
		}

}

void IcnChannel::clear_stat(){
    count = 0;
}


double IcnChannel::get_cost(){
    return count*price;
}

Register_Class(IcnChannel);
//</aa>
