#include "PlayerControl.h"
#include "Scene.h"
#include "transform3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/ext.hpp"

using namespace glm;

//PlayerControl [base]

PlayerControl::PlayerControl()
{
	speed = 20.0f;
	rad = 0.4f;
	ground_snap = 0.12f;
	last_update = 0;
}

void PlayerControl::update()
{	}

void PlayerControl::receive_input(const PInput& input)
{	}

void PlayerControl::receive_state(const PState& s)
{	}

void PlayerControl::receive(Packet* p)
{	}

void PlayerControl::send(Packet* p)
{	}

void PlayerControl::update_state(const float d, const PInput& input)
{
	//Orientation

	state->mx = input.mx;
	state->my = input.my;

	mat4 mRot = rotate(mat4(), state->mx, UP);

	vec3 dir = transform3(mRot, vec3(0.0f, 0.0f, 1.0f));
	vec3 right = cross(dir, UP);
	

	//Movement

	vec2 md(0);

	if(input.up())
		md.y++;
	if(input.down())
		md.y--;
	if(input.left())
		md.x--;
	if(input.right())
		md.x++;

	if(md.x * md.x + md.y * md.y > 0) //md.LengthSquared() > 0
	{
		md = normalize(md);
		state->vel -= (dir * md.y + right * md.x) * speed * d;
	}


	//Jumping

	if(state->on_ground)
	{
		if(input.jump())
		{
			state->vel.y = 2.5f;
			state->force_detach_time = 0.1f;
		}
	}
	

	//Static sphere/triangle collision for body

	vec3 cpts[200];
	int cpc = scene->getPoints(state->pos, rad + 0.2f, cpts, 200);

	vec3 vd; float s; float radSquared = rad * rad; mat4 m;
	for(vec3* v = cpts; v < cpts + cpc; v++)
	{
		vd = state->pos - *v;
		s = DOT(vd, vd); //vd.LengthSquared()

		if(s < radSquared)
		{
			vd = normalize(vd);
			state->pos += (rad - sqrt(s)) * vd;

			/*
			//Create matrix to collision-plane space
			m = lookAt(vec3(0.0f), vd, abs(vd.y) > 0.5f ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f));

			//Transform vel to collision-plane space
			state->vel = transform3(m, state->vel);

			//Flatten the velocity in collision-plane space
			state->vel.z = 0;

			//Transform vel back to world space
			state->vel = transform3(inverse(m), state->vel);*/
		}
	}


	//Static ray/triangle collision for feet

	float u, v, t;
	
	state->on_ground = false;

	if(scene->rayCastStatic(state->pos, DOWN, &t, &u, &v, false))
	{
		if( state->on_ground = (t < rad + ground_snap && state->vel.y <= 0.0f) )
		{
			state->pos.y += (rad + ground_snap - 0.05f) - t;
			state->vel.y = 0.0f;
		}
	}

	//TEMPORARY FRICTION

	state->vel.z *= 1.0f - d * 5.0f;
	state->vel.x *= 1.0f - d * 5.0f;

	//Integration

	state->vel += scene->gravity * d;
	state->pos += state->vel * d;

	//Update the time

	state->u_time += d;
}


//PlayerControlServer [derived]

PlayerControlServer::PlayerControlServer()
{
	PlayerControl::PlayerControl();

	last_time = 0.0f;

	current_state = PState();
	current_input = PInput();
	state = &current_state;
}

void PlayerControlServer::receive_input(const PInput& input)
{
	float _dt = /*s_time - last_time*/ 1.0f / 60.0f;

	if(_dt < 0.1f)
		update_state(_dt, input);

	current_input = input;

	last_time = s_time;
}

void PlayerControlServer::receive(Packet* p)
{
	receive_input(PInput(p));
}

void PlayerControlServer::send(Packet* p)
{
	current_state.send(p);	
	current_input.send(p);
}

void PlayerControlServer::send_predicted(Packet* p)
{
	p->writeUint(current_input.index);
	current_state.send(p);
}


//PlayerControlClient [derived]

PlayerControlClient::PlayerControlClient()
{
	PlayerControl::PlayerControl();

	current_input = PInput();

	current_state = target_state = PState();
	state = &current_state;
}

void PlayerControlClient::receive_state(const PState& s)
{
	target_state = s;
}

void PlayerControlClient::receive_input(const PInput& i)
{
	current_input = i;
}

void PlayerControlClient::receive(Packet* p)
{
	receive_state(PState(p));
	receive_input(PInput(p));
}

void PlayerControlClient::update()
{
	//Interpolate the state

	//Position
	vec3 pos_diff = target_state.pos - current_state.pos;
	float pd_lsq = DOT(pos_diff, pos_diff);
	if(pd_lsq > 0.5f * 0.5f)
		current_state.pos = target_state.pos;
	else if(pd_lsq > 0.1f)
		current_state.pos += pos_diff * 0.1f;

	//Mouse X
	float mx_diff = target_state.mx - current_state.mx;
	if(mx_diff > 1500.0f)
		current_state.mx = target_state.mx;
	else if(mx_diff > 0.1f)
		current_state.mx += mx_diff * 0.1f;

	//Mouse Y
	float my_diff = target_state.my - current_state.my;
	if(my_diff > 1500.0f)
		current_state.my = target_state.my;
	else if(my_diff > 0.1f)
		current_state.my += my_diff * 0.1f;

	//Velocity
	current_state.vel = target_state.vel;

	//Control variables
	current_state.on_ground = target_state.on_ground;
	current_state.force_detach_time = target_state.force_detach_time;


	//Predict the state

	update_state(dt, current_input);
}


//PlayerControlClientPredicted [derived]

PlayerControlClientPredicted::PlayerControlClientPredicted()
{
	PlayerControl::PlayerControl();

	move_head = move_tail = move_buffer;
	move_end = move_buffer + buffer_size;

	state = &move_tail->state;

	correction_treshold = 0.3f;

	for(PMove* m = move_buffer; m < move_end; m++)
	{
		*m = PMove();
	}
}


inline float lengthSq(vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}
void PlayerControlClientPredicted::receive_state(const PState& c_state, const unsigned int index)
{

	while (index != move_head->input.index && move_head != move_tail)
		move_head = next(move_head);

	if(true)
	{
		//printf("%d == %d\n", index, move_head->input.index);
		//printf("WOO: %f > %f\n", lengthSq(move_head->state.pos - c_state.pos), correction_treshold * correction_treshold, abs(c_state.u_time - move_head->state.u_time));

		float len = lengthSq(move_head->state.pos - c_state.pos);

		if(len > correction_treshold * correction_treshold)
		{
			move_tail->state = c_state;
			/*move_head->state = c_state;

			float currentTime = c_state.u_time;
			
			move_head = next(move_head);

			PMove* index = move_head;

			while (index != next(move_tail))
			{
				const float _dt = index->state.u_time - currentTime;
			
				if(_dt == 0)
					continue;

				state = &index->state;
				update_state(_dt, index->input);
				index = next(index);

				currentTime = index->state.u_time;
			}
			move_tail->state = index->state;
			state = &move_tail->state;*/
		}
	}
}

void PlayerControlClientPredicted::receive_input(const PInput& i)
{
	move_tail->input = i;
}

void PlayerControlClientPredicted::update()
{
	state = &move_tail->state;
	update_state(1.0f / 60.0f, move_tail->input);

	PMove* m = move_tail;
	move_tail = next(move_tail);
	move_tail->state = m->state;
}

void PlayerControlClientPredicted::receive(Packet* p)
{
	unsigned int index = p->readUint();
	receive_state(PState(p), index);
}


//PlayerControlClientPredicted::advance [helper]


inline PMove* PlayerControlClientPredicted::next(PMove* move)
{
	move++;
	if(move == move_end)
		move = move_buffer;
	return move;
}

inline PMove* PlayerControlClientPredicted::prev(PMove* move)
{
	if(move == move_buffer)
		move = move_end - 1;
	else
		move--;
	return move;
}