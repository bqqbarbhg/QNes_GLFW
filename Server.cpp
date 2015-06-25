#include "Server.h"
#include "Scene.h"
#include <stdio.h>


#include "servercontrol.h"
#include "EntityPlayer.h"

bool server = true;
unsigned int frame = 0;

//HM
#include "EntityModelStatic.h"
#include "EntityPlayerControlProto.h"



Server::Server(int max)
{
	conn_ptr = new VirtualConnection*[max];
	running = true;
	server = true;
	keyb_input = true;
	conn_c = 0;
	old_time = 0.0f;
	scene->addAddCallback(new EntityCallback<Server>(this, &Server::addEntityCallback));
	glfwInit();
}

void Server::addEntityCallback(Entity *e)
{
	for(VirtualConnection** vc = conn_ptr; vc < conn_ptr + conn_c; vc++)
	{
		VirtualConnection* v = *vc;
		if(v->state->has_scene)
		{
			e->write(v);
		}
	}
}

void Server::run()
{
	scene->add(new EntityModelStatic(Model::loadNoTex("tscene.omf")));
	while(running)
	{
		update_dt();

		receive_packets();

		if(TICK(0, 2))
			send_packets();

		update_connections();

		scene->update();

		glfwSleep(1.0f / 60.0f);
	}
}

void Server::update_dt()
{
	s_time = (float)glfwGetTime();
	dt = s_time - old_time;
	old_time = s_time;

	frame++;
}

void Server::receive_packets()
{
	Address a;
	Packet p;

	while (net_socket->receive(a, p))
	{
		if(p.readHeader())
		{
			VirtualConnection* senderConnection;

			if(senderConnection = getVC(a))
			{
				senderConnection->readSeq(p);

				switch(p.readChar())
				{
					case NET_OP_REQ_SCENE:
						senderConnection->state->has_scene = true;
						printf("Client connected: %d.%d.%d.%d:%d\n", a.ip_1, a.ip_2, a.ip_3, a.ip_4, a.ip_port);
						scene->write(senderConnection);
						break;
					case NET_OP_HEARTBEAT:
						break;
					case NET_OP_SEND_INPUT:
						
						{
							int skip = p.readUint();

							PlayerControl* e = reinterpret_cast<PlayerControl*>(senderConnection->state->player);

							if(e->last_update < senderConnection->seq_num_remote)
							{
								e->receive(&p);
								e->last_update = senderConnection->seq_num_remote;
							}
							else
								p.dptr += skip;
						}

						break;

					case NET_OP_CHAT_MESSAGE:

						char* str = p.readString();
						printf("::%s\n", str);

						for(VirtualConnection** vv = conn_ptr; vv < conn_ptr + conn_c; vv++)
						{
							VirtualConnection* v = *vv;
							
							Packet* p = v->packet_manager->start(NET_OP_CHAT_MESSAGE);

							p->writeString(str);

							v->packet_manager->end();

						}

						delete [] str;

						break;

				}
			}
			else
			{
				printf("Client trying to connect: %d.%d.%d.%d:%d\n", a.ip_1, a.ip_2, a.ip_3, a.ip_4, a.ip_port);

				conn_ptr[conn_c] = VirtualConnection::approve(a, p);
				conn_ptr[conn_c]->requestConnection();
				conn_ptr[conn_c]->state = new ConnectionState();
				

				EntityPlayer* p = new EntityPlayer(new PlayerControlServer(), a);
				conn_ptr[conn_c]->state->player = p->control;
				p->owned = true;
				p->control->state->pos = glm::vec3(0.0f, 5.0f, 0.0f);
				scene->add(p);

				conn_c++;
				
			}
		}
		delete [] p.data; p.data = NULL;
	}


	
}

void Server::send_packets()
{
	for(VirtualConnection** vv = conn_ptr; vv < conn_ptr + conn_c; vv++)
	{
		VirtualConnection* v = *vv;

		//printf("Reliable packets: %d\n", v->reliablePackets.size());

		scene->send(v);

		v->packet_manager->start(NET_OP_HEARTBEAT);
		v->packet_manager->end();

		v->sendManagedPackets();
			
		//v->printPacketList();
	}
	//printf("\n");
}

void Server::update_connections()
{
	for(VirtualConnection** vv = conn_ptr; vv < conn_ptr + conn_c; vv++)
	{
		VirtualConnection* v = *vv;

		v->updateReliablePackets(dt);

		if(v->last_recv > 10.0f)
		{
			/*
				conn_ptr = 0xAAAAAAAA
				vv = 0xAAAAAAAA + index
				index = vv - conn_ptr

				dest = vv
				src = vv + 1
				length = conn_c - index - 1
				length[bytes] = length * sizeof(Q*)
			*/

			conn_c--;

			printf("Client disconnected (timeout): %d.%d.%d.%d:%d\n", v->address.ip_1, v->address.ip_2, v->address.ip_3, v->address.ip_4, v->address.ip_port);

			memcpy(vv, vv + 1, (conn_c - (vv - conn_ptr)) * sizeof(VirtualConnection*));

			delete v;

			return;
		}
	}
}

VirtualConnection* Server::getVC(Address a)
{
	for(VirtualConnection** vv = conn_ptr; vv < conn_ptr + conn_c; vv++)
	{
		VirtualConnection* v = *vv;

		if(v->address == a)
			return v;
	}
	return NULL;
}

int Server::getID(Address a)
{
	int i = 0;
	for(VirtualConnection** vv = conn_ptr; vv < conn_ptr + conn_c; vv++)
	{
		VirtualConnection* v = *vv;
		
		if(v->address == a)
			return i;
		i++;
	}
	return -1;
}