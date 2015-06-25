#pragma once

#include "virtualconnection.h"

class Client
{
public:
	Client(Address address);
	
	void run();

	int scw, sch;
	VirtualConnection* connection;

private:
	bool connecting, new_load, running;

	float old_time;
	void process_packets();
	void update_connect();
	void draw_connect();
	void request_scene();
	void update_game();
	void draw_game();
	void draw_console();
	void initGlfwGlew();
	void update_dt();
	void check_timeout();
	void send_state();
	void handle_console();
	void send_packets();
};