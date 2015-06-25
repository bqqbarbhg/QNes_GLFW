#include "network.h"

class VirtualConnection;
class PacketManager
{
public:
	PacketManager();
	~PacketManager();

	Packet* start();
	Packet* start(char proc);
	Packet* startDlen(char proc);
	void end();
	void endDlen();
	
	void send(VirtualConnection* vc);

	int packet_c;
	ReliablePacket* packet_ptr;

private:
	Packet* temp;
};