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


Define_Channel(IcnChannel);

void IcnChannel::initialize()
{
        cDatarateChannel::initialize();
        cost = par("cost");
        double cost_asc = par("cost_asc");
        double cost_desc = par("cost_desc");
        
        //cGate* sourceGate = getSourceGate();
        //cModule* node_ = sourceGate->getOwnerModule();
        //int source_index = node_->getIndex();
        
        int source_index = getSourceGate()->getOwnerModule()->getIndex();
        int dest_index = getSourceGate()->getNextGate()->getOwnerModule()->getIndex();
        cost = source_index < dest_index ? cost_asc : cost_desc;
        
        effectiveCostID = registerSignal("effectiveCost");
        #ifdef SEVERE_DEBUG
        std::stringstream msg; 
		msg<<"cost for each data pkt from node["<<source_index<<"] to node["
			<<dest_index<<"]: "<< cost;
	    debug_message(__FILE__,__LINE__,msg.str().c_str() );
	    #endif
	    systemIsStable = false;
}


void IcnChannel::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
        cDatarateChannel::processMessage(msg,t,result);
        if( msg->getKind() == CCN_D && systemIsStable){
        	// This is a data packet
        	emit(effectiveCostID, cost );
        }
}

void IcnChannel::notifyStability(){
	systemIsStable = true;
}
//</aa>
