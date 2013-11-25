/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Andrea Araldo (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Dario Rossi (occasional debugger, mailto dario.rossi@enst.fr)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CCN_NODE_H
#define CCN_NODE_H
//<aa>
#ifndef CHANNELWITHCOST_H_
#define CHANNELWITHCOST_H_

#include <cdataratechannel.h>

class ChannelWithCost: public cDatarateChannel {
    private:
        simsignal_t effectiveCostID;

    protected:
        double cost;
        virtual void initialize();
        virtual void processMessage(cMessage *msg, simtime_t t, result_t& result);
};

#endif /* CHANNELWITHCOST_H_ */
//</aa>
#endif /* CCN_NODE_H */