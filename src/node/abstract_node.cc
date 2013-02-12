/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
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

#include "abstract_node.h"
AbstractNode::AbstractNode():cSimpleModule(){
    ;
}

cModule *AbstractNode::findSibling(string mod_name){
    return getParentModule()->getModuleByRelativePath(mod_name.c_str());
}

int AbstractNode::getOuterInterfaces(){
    return getParentModule()->gateSize("face");
}

int AbstractNode::getIndex(){
    return getParentModule()->getIndex();
}

bool AbstractNode::check_client(uint32_t interface){
    client *c;
    bool check= false;
    c = dynamic_cast<client *>(getParentModule()->gate("face$o",interface)->getNextGate()->getOwnerModule());
    if (c)
        check=true;
    return check;
}

