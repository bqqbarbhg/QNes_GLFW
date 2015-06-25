#pragma once

#include "glm/glm.hpp"
#include "network.h"
#include "virtualconnection.h"

#define BIT(a) (1 << (a))

struct PInput
{
	static const unsigned int packet_size = sizeof(char) + sizeof(float) * 2 + sizeof(unsigned int);

	PInput()
		:wasd(0), mx(0), my(0) {}
	PInput(Packet* p);

	char wasd;
	float mx, my;

	unsigned int index;

	void send(Packet* p);

	bool up() const { return wasd & BIT(0); }
	bool left() const { return wasd & BIT(1); }
	bool down() const { return wasd & BIT(2); }
	bool right() const { return wasd & BIT(3); }
	bool jump() const { return wasd & BIT(4); }
};

struct PState
{
	static const unsigned int packet_size = sizeof(glm::vec3) * 2 + sizeof(float) * 3;

	PState()
		: force_detach_time(0.0f), on_ground(false), pos(0.0f),
			vel(0.0f), mx(0.0f), my(0.0f), u_time(0.0f)
	{ }

	PState(Packet* p);

	glm::vec3 pos;
	glm::vec3 vel;

	bool on_ground;
	float force_detach_time;

	float mx, my;
	float u_time;

	static PState lerp(const PState& s1, const PState& s2, const float amount);

	void send(Packet* p);

	glm::vec3 getDir();
	glm::vec3 getFDir();
	glm::vec3 getRight();

};

struct PMove
{
	PMove() : state(), input() {}

	PState state;
	PInput input;
};

