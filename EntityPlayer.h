#pragma once

#include "Entity.h"
#include "PlayerControl.h"
#include "virtualconnection.h"

class EntityPlayer : public Entity
{
public:
	EntityPlayer(PlayerControl* c);
	EntityPlayer(PlayerControl* c, Address a);

	~EntityPlayer();

	PlayerControl* control;

	virtual void update();
	virtual void draw();

	virtual void write(VirtualConnection* v);
	static EntityPlayer* create(VirtualConnection* v, Packet& p);

	virtual void send(VirtualConnection* v);
	virtual void receive(VirtualConnection* v, Packet& p);

	bool owned;
	Address owner;
};