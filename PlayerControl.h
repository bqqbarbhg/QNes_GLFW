#pragma once

#include "playerstate.h"
#include "network.h"
#include "virtualconnection.h"

class PlayerControl
{
public:
	PlayerControl();

	PState* state;
	float speed;
	float rad;
	float ground_snap;
	int index;
	unsigned int last_update;

	virtual void update();
	virtual void receive_state(const PState& state);
	virtual void receive_input(const PInput& input);
	virtual void update_state(const float dt, const PInput& input);

	virtual void send(Packet* p);
	virtual void receive(Packet* p);
};




class PlayerControlServer : public PlayerControl
{ 
public:
	PlayerControlServer();

	virtual void receive_input(const PInput& input);

	virtual void send(Packet* p);
	virtual void send_predicted(Packet* p);
	virtual void receive(Packet* p);

private:
	PState current_state;
	PInput current_input;
	float last_time;

};




class PlayerControlClient : public PlayerControl
{
public:
	PlayerControlClient();

	virtual void update();
	virtual void receive_state(const PState& state);
	virtual void receive_input(const PInput& input);

	virtual void receive(Packet* p);

private:
	PState current_state, target_state;
	PInput current_input;
};




class PlayerControlClientPredicted : public PlayerControl
{
public:
	PlayerControlClientPredicted();

	float correction_treshold;

	virtual void update();
	virtual void receive_input(const PInput& input);
	virtual void receive_state(const PState& state, const unsigned int index);

	virtual void receive(Packet* p);

private:
	static const int buffer_size = 1024;
	PMove *move_head, *move_tail, *move_end;
	PMove move_buffer[buffer_size];

	inline PMove* next(PMove* move);
	inline PMove* prev(PMove* move);
};