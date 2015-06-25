#include "virtualconnection.h"

using namespace std;

VirtualConnection::VirtualConnection(const Address& a)
{
	key = rand();
	address = a;
	seq_num_remote = ack_bitfield = 0;
	seq_num_local = 1;
	last_recv = 0.0f;
	packet_manager = new PacketManager();
	state = NULL;
}

VirtualConnection::~VirtualConnection()
{
	delete packet_manager;

	if(state)
		delete state;
}


VirtualConnection* VirtualConnection::request(const Address& address)
{
	int psize = PACKET_HEADERSZ_RELIABLE + sizeof(int);

	VirtualConnection* vc = new VirtualConnection(address);

	ReliablePacket p = ReliablePacket(psize);
	p.writeHeader();
	vc->writeSeq(p);
	p.writeChar(NET_OP_REQ_VC);
	p.writeInt(vc->key);

	vc->sendReliablePacket(p);

	return vc;
}

void VirtualConnection::requestConnection()
{
	int psize = PACKET_HEADERSZ_RELIABLE + sizeof(int);

	ReliablePacket p = ReliablePacket(psize);
	p.writeHeader();
	writeSeq(p);
	p.writeChar(NET_OP_REQ_VC);
	p.writeInt(key);
	
	sendReliablePacket(p);
}

VirtualConnection* VirtualConnection::approve(const Address& a, Packet& p)
{
	VirtualConnection* vc = new VirtualConnection(a);

	vc->readSeq(p);
	char c = p.readChar();
	assert(c == NET_OP_REQ_VC);

	vc->key = p.readInt();
	
	return vc;
}

void VirtualConnection::sendReliablePacket(ReliablePacket& packet)
{
	packet.seq_num = seq_num_local;

	//packet_state_head = (packet_state_head + 1) % packet_state_c;

	reliablePackets.push_back(packet);

	net_socket->send(address, packet);

	packet.data = NULL;
}

void VirtualConnection::writeSeq(Packet& p)
{
	p.writeUint(seq_num_local);
	p.writeUint(seq_num_remote);
	p.writeUint(ack_bitfield);

	seq_num_local++;
}

void VirtualConnection::readSeq(Packet& p)
{
	unsigned int sr = p.readUint(); //Remote sequence number

	assert(sr != seq_num_remote);	
	
	if(sr > seq_num_remote)
	{
		int d = sr - seq_num_remote;
		ack_bitfield <<= d;
		ack_bitfield |= 1 << (d - 1);
		seq_num_remote = sr;
	}
	else if(sr >= seq_num_remote - 33)
	{
		ack_bitfield |= 1 << (seq_num_remote - sr - 1);
	}

	unsigned int sl = p.readUint(); //Local sequence number
	unsigned int ab = p.readUint(); //Ack bitfield

	for(list<ReliablePacket>::iterator it = reliablePackets.begin(); it != reliablePackets.end();)
	{
		//Check if the received sequence number is more recent than the packet
		if(sl >= it->seq_num)
		{
			unsigned int d = sl - it->seq_num;
		
			if(d == 0 || (d <= 33 && (ab & (1 << (d - 1)))))
			{
				if(reliablePackets.size() > 1)
					it = reliablePackets.erase(it);
				else
				{
					reliablePackets.erase(it);
					break;
				}
				
			}
			else
			{
				++it;
			}
		}
		else
			++it;
	}

	last_recv = 0.0f;

}

void VirtualConnection::updateReliablePackets(float dt)
{
	last_recv += dt;

	//printf("%d, %d, %d\n", seq_num_local, seq_num_remote, ack_bitfield);
	for(list<ReliablePacket>::iterator it = reliablePackets.begin(); it != reliablePackets.end(); ++it)
	{
		
		
		
			it->age += dt;
		if(it->age > 1.1f)
		{
			//Resend the packet
			it->seq_num = seq_num_local;
			it->age = 0;
			it->reset();
			it->writeHeader();
			writeSeq(*it);

			net_socket->send(address, *it);
		}
	}
}

void VirtualConnection::sendManagedPackets()
{
	packet_manager->send(this);
}

void VirtualConnection::printPacketList()
{
	/*printf("Packets: %d\n", reliablePackets.size());
	printf("Local %d, Remote %d\n", seq_num_local, seq_num_remote);
	
	for(int i = 0; i < 32; i++)
	{
		printf((ack_bitfield >> i) & 1 ? "1" : "0");
	}*/

	printf("%5d ", reliablePackets.size());

	//printf("\n");
	/*for(list<ReliablePacket>::iterator it = reliablePackets.begin(); it != reliablePackets.end(); ++it)
	{
		//printf("%d: %d  %f\n", i, it->seq_num_remote, it->age);
		i++;
	}*/
}