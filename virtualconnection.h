#pragma once

#include "network.h"
#include "packetmanager.h"
#include <list>

class ConnectionState;

class VirtualConnection
{
public:
	//Const and deconst
	VirtualConnection(const Address& address);
	~VirtualConnection();


	//Fields
	std::list<ReliablePacket> reliablePackets;
	PacketManager* packet_manager;
	Address address;
	int key;

	unsigned int seq_num_local;
	unsigned int seq_num_remote;
	unsigned int ack_bitfield;

	float last_recv;

	//Static methods
	static VirtualConnection* request(const Address& address);
	static VirtualConnection* approve(const Address& address, Packet& packet);

	ConnectionState* state;

	//Methods
	void requestConnection();
	void sendReliablePacket(ReliablePacket& packet);
	void addPacketHandle(char* ptr);
	void writeSeq(Packet& p);
	void readSeq(Packet& p);

	void updateReliablePackets(float dt);
	void sendManagedPackets();

	void printPacketList();
};