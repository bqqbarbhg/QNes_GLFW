#pragma once

#include "network.h"
#include "virtualconnection.h"
#include "ValueSynchronizer.h"

const char ENTITY_TYPE_ENTITY_MODEL_STATIC = 0;
const char ENTITY_TYPE_ENTITY_PLAYER_CONTROL_PROTO = 1;
const char ENTITY_TYPE_ENTITY_PLAYER = 2;

class Scene;

class Entity{
public:
	Entity();
	bool dead;

	virtual void init();
	virtual void update();
	virtual void draw();
	virtual int getPacketSize();
	virtual void send(VirtualConnection* v);
	virtual void receive(VirtualConnection* v, Packet& packet);

	virtual void write(VirtualConnection* v);
	virtual void read(VirtualConnection* v, Packet& p);
	static Entity* create(VirtualConnection* v, Packet& p);
	unsigned int last_update;
	int id;
};