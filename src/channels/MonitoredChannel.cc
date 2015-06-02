#include "channels/MonitoredChannel.h"
Register_Class (MonitoredChannel);

void MonitoredChannel::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
	std::cout<<"ciao"<<endl;
	exit(1);
	cIdealChannel::processMessage(msg, t, result);
}


