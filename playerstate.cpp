#include "playerstate.h"
#include "glm/gtc/matrix_transform.hpp"
#include "transform3.h"
#include "glm/ext.hpp"

using namespace glm;

//PInput

PInput::PInput(Packet* p)
{
	index = p->readUint();
	wasd = p->readChar();
	mx = p->readFloat();
	my = p->readFloat();
}

void PInput::send(Packet* p)
{
	p->writeUint(index);
	p->writeChar(wasd);
	p->writeFloat(mx);
	p->writeFloat(my);
}


//PState

PState::PState(Packet* p)
	: force_detach_time(0.0f), on_ground(false)
{
	pos = p->readVec3();
	vel = p->readVec3();

	mx = p->readFloat();
	my = p->readFloat();

	u_time = p->readFloat();
}

void PState::send(Packet* p)
{
	p->writeVec3(pos);
	p->writeVec3(vel);
	
	p->writeFloat(mx);
	p->writeFloat(my);

	p->writeFloat(u_time);
}

PState PState::lerp(const PState& s1, const PState& s2, const float amount)
{
	PState l;
	
	l.pos		= glm::lerp(	s1.pos,		s2.pos,		amount	);
	l.vel		= glm::lerp(	s1.vel,		s2.vel,		amount	);
	l.mx		= glm::lerp(	s1.mx,		s2.mx,		amount	);
	l.my		= glm::lerp(	s1.my,		s2.my,		amount	);
	l.u_time	= glm::lerp(	s1.u_time,	s2.u_time,	amount	);
	
	l.on_ground = amount < 0.5f ? s1.on_ground : s2.on_ground;
	l.force_detach_time = glm::lerp(	s1.force_detach_time,	 s2.force_detach_time,	 amount);

	return l;
}

//Flat transformed vectors

vec3 PState::getFDir()
{ return transform3(rotate(mat4(), mx, UP), FORWARD); }

vec3 PState::getRight()
{ return transform3(rotate(mat4(), mx, UP), RIGHT); }


//Non-flat transformed vectors

vec3 PState::getDir()
{ return transform3(rotate(rotate(mat4(), mx, UP), my, RIGHT), FORWARD); }