#if 0

#include "Entity.h"
#include "glm/glm.hpp"

class EntityPlayerControlProto : public Entity
{
public:
	glm::vec3 pos, vel;

	bool onGround;
	Address owner;
	EntityPlayerControlProto();
	EntityPlayerControlProto(glm::vec3 pos);
	EntityPlayerControlProto(glm::vec3 pos, Address owner);
	~EntityPlayerControlProto();

	virtual void update();
	virtual void draw();

	virtual void write(VirtualConnection* c);
	static EntityPlayerControlProto* create(VirtualConnection* c, Packet& p);

	virtual void send(VirtualConnection* c);
	virtual void receive(VirtualConnection* c, Packet& p);
	

	glm::vec2 cre;

	bool owned;
	ConnectionState* state;

private:
	glm::vec2 oldMouse;
	glm::vec3 dir, up;
	glm::vec3* cpts;
	glm::vec3 fdir, right;
	

	inline void initn();
};

#endif