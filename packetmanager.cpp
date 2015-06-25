#include "virtualconnection.h"

PacketManager::PacketManager()
{
	temp = new Packet(MAX_PACKET_SIZE);
	packet_ptr = new ReliablePacket[32]; //64kB
	for(ReliablePacket* p = packet_ptr; p < packet_ptr + 32; p++)
	{
		p->data = new char[MAX_PACKET_SIZE];
		p->dptr = p->data;
	}
	packet_c = 0;
}

PacketManager::~PacketManager()
{
	delete temp;
	delete [] packet_ptr;
}

Packet* PacketManager::start()
{
	temp->dptr = temp->data;
	return temp;
}

Packet* PacketManager::start(char proc)
{
	temp->dptr = temp->data;
	temp->writeChar(proc);
	return temp;
}

Packet* PacketManager::startDlen(char proc)
{
	temp->dptr = temp->data;
	temp->writeChar(proc);
	temp->dptr += sizeof(unsigned int);
	return temp;
}

void PacketManager::end()
{
	int tsize = temp->dptr - temp->data;
	
	ReliablePacket* p;

	if(packet_c == 0 || ((packet_ptr + packet_c - 1)->dptr - (packet_ptr + packet_c - 1)->data) < tsize)
	{
		p = packet_ptr + packet_c;
		p->dptr = p->data + (PACKET_HEADERSZ_RELIABLE - sizeof(char));
		packet_c++;
	}
	else
		p = packet_ptr + packet_c - 1;

	memcpy(p->dptr, temp->data, tsize); p->dptr += tsize;
}

void PacketManager::endDlen()
{
	unsigned int tsize = temp->dptr - temp->data - sizeof(char) - sizeof(unsigned int);

	memcpy(temp->data + sizeof(char), &tsize, sizeof( unsigned int ));

	end();
}

void PacketManager::send(VirtualConnection* vc)
{
	for(ReliablePacket* p = packet_ptr; p < packet_ptr + packet_c; p++)
	{
		p->length = p->dptr - p->data;

		p->dptr = p->data;
		p->writeHeader();
		vc->writeSeq(*p);

		vc->sendReliablePacket(ReliablePacket(p->length, (char*)memcpy(malloc(p->length), p->data, p->length)));
	}
	packet_c = 0;
}
