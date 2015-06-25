#include <stdio.h>
#include <iostream>

#include "Client.h"
#include "Server.h"

#include "Camera.h"
#include "Scene.h"

#include "stringop.h"

#if _DEBUG
#include <vld.h>
#endif

using namespace std;

Camera* camera = new Camera();
Scene* scene = new Scene();
float dt;

Socket* net_socket = new Socket();

void runClient(Address);
void runServer(int);

const int port = 40006;

int main(int argc, char** argv)
{
	char* line = new char[100];
	bool inc = true;

	while(inc)
	{
		cin.getline(line, 100);
		
		if( strcmpl(line, "a "))
		{
			Address m;

			sscanf(line, "a %d.%d.%d.%d:%d", &m.ip_1, &m.ip_2, &m.ip_3, &m.ip_4, &m.ip_port);
			
			m.packIP();

			runClient( m );

		}

		if(!strcmp(line, "c"))
		{
			runClient(Address(127, 0, 0, 1, port));
		}

		if(!strcmp(line, "s"))
		{
			runServer(32);
		}
	}

	return 0;
}

void runClient(Address a)
{
	initSockets();

	net_socket->open(0);

	Client client = Client(a);

	client.run();
}

void runServer(int max_players)
{
	initSockets();

	net_socket->open(port);

	Server server = Server(max_players);

	server.run();
}