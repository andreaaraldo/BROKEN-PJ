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

package modules.channels;

import ned.DatarateChannel;
import ned.IdealChannel;

channel IcnChannel extends DatarateChannel
{
    parameters:
        @class(IcnChannel);        
    	
    	// Cost that the destination node must pay for each data packet
    	// received through this channel
        double price = default(0);
        
        // Suppose that this IcnChannel goes out from a node of index i_source and 
        // enters the node of index i_dest. In IcnChannel::initialization, cost will be 
        // set as follows:
        //		cost = i_source < i_dest cost_up ? cost_asc : cost_desc;
        double price_asc = default(0);
        double price_desc = default(0);
}
