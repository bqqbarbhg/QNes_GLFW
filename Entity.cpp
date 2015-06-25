#include "Entity.h"
#include "EntityModelStatic.h"
#include "EntityPlayerControlProto.h"
#include "EntityPlayer.h"

Entity::Entity()
{
	dead = false;
	last_update = 0;
}

void Entity::update()
{

}
void Entity::draw()
{

}
void Entity::send(VirtualConnection* p)
{
	//sync.send(p);
}
void Entity::receive(VirtualConnection* v, Packet& p)
{
	//sync.receive(p);
}
int Entity::getPacketSize()
{
	return -1;
}
void Entity::init()
{
	
}
void Entity::write(VirtualConnection* v)
{

}
void Entity::read(VirtualConnection* v, Packet& p)
{

}
Entity* Entity::create(VirtualConnection* v, Packet& p)
{
	switch(p.readChar())
	{
	case ENTITY_TYPE_ENTITY_MODEL_STATIC:
		return EntityModelStatic::create(v, p);
/*	case ENTITY_TYPE_ENTITY_PLAYER_CONTROL_PROTO:
		return EntityPlayerControlProto::create(v, p);	*/
	case ENTITY_TYPE_ENTITY_PLAYER:
		return EntityPlayer::create(v, p);
	}
	return new Entity;
}