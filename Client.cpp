#include "Client.h"
#include "Scene.h"

#ifndef _DEBUG
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GL/glfw.h>



//REMOVE US
#include "EntityModelStatic.h"
#include "EntityPlayerControlProto.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "transform3.h"
#include "textrender.h"
#include "servercontrol.h"
#include "PlayerControl.h"
#include "playerstate.h"

using namespace glm;

unsigned int input_index = 0;
float mx = 0, my = 0;
float s_time;
bool keyb_input;
const int console_recv_max = 8;
const int console_max = 256;
char console_buffer[console_max];
int console_i = 0;
char console_recv_buffer[console_max * console_recv_max];
float console_recv_age[console_recv_max];
char* console_ptr;

bool old_key[0xff];

void Client::initGlfwGlew()
{
	glfwInit();


	
	if(!glfwOpenWindow(scw, sch, 0, 0, 0, 0, 26, 8, GLFW_WINDOW))
	{
		glfwTerminate();
	}

	glfwSwapInterval(1);

	glewInit();
	glfwSetWindowTitle("Q N.E.S");
}

Client::Client(Address address)
{
	scw = 800;
	sch = 600;

	running = true;
	keyb_input = true;
	server = false;

	connection = VirtualConnection::request(address);
	connection->state = new ConnectionState();

	for(float* f = console_recv_age; f < console_recv_age + console_recv_max; f++)
	{
		*f = 0.0f;
	}

	old_time = 0.0f;

	initTextRender();

	memset(old_key, 0, 0xff);
	memset(console_recv_buffer, 0, console_max * console_recv_max);

	connecting = true;
	new_load = true;
}

void Client::run()
{
	initGlfwGlew();

	while(!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED) && running)
	{
		update_dt();

		if(connection != NULL)
		{
			connection->updateReliablePackets(dt);
		}

		if(connecting)
		{
			update_connect();

			draw_connect();
		}
		else
		{
			handle_console();

			if(TICK(0, 2))
				send_packets();

			send_state();

			update_game();

			draw_game();

			draw_console();

			glfwSwapBuffers();
		}
		
		//connection->printPacketList();

	}

	glfwTerminate();
}

void Client::send_packets()
{
	

	connection->packet_manager->start(NET_OP_HEARTBEAT);
	connection->packet_manager->end();

	if(connection != NULL)
		connection->sendManagedPackets();
}

PInput get_input()
{
	PInput input = PInput();
	
	if(keyb_input)
	{
		input.wasd |= glfwGetKey('W');
		input.wasd |= glfwGetKey('A')				<< 1;
		input.wasd |= glfwGetKey('S')				<< 2;
		input.wasd |= glfwGetKey('D')				<< 3;
		input.wasd |= glfwGetKey(GLFW_KEY_SPACE)	<< 4;

		int imx, imy;
		glfwGetMousePos(&imx, &imy);


		mx += (300.0f - (float)imx) * 0.5f;
		my += (300.0f - (float)imy) * 0.5f;
	
		if (!glfwGetKey('Q'))
			glfwSetMousePos(300, 300);
	}
	input.mx = mx;
	input.my = my;

	input.index = input_index++;

	return input;
}

void Client::send_state()
{
	//Manually send input 60 times a second
	Packet p = Packet(PACKET_HEADERSZ_RELIABLE + sizeof(char) + sizeof(unsigned int) + PInput::packet_size);

	p.writeHeader();
	connection->writeSeq(p);

	p.writeChar(NET_OP_SEND_INPUT);

	p.writeUint(PInput::packet_size);

	PInput input = get_input();

	input.send(&p);
	if(connection->state->player)
		reinterpret_cast<PlayerControl*>(connection->state->player)->receive_input(input);

	net_socket->send(connection->address, p);
}



void Client::handle_console()
{
	for(float* f = console_recv_age; f < console_recv_age + console_recv_max; f++)
	{
		*f -= dt;
	}

	if(!keyb_input)
	{
		bool b;
		for(char c = 'A'; c <= 'Z'; c++)
		{
			b = glfwGetKey(c);
			if(b && !old_key[c] && console_ptr - console_max - 1 != console_buffer)
			{
				*console_ptr++ = c;
				console_ptr[1] = 0;
			}
			old_key[c] = b;
		}

		bool sb = glfwGetKey(GLFW_KEY_SPACE);

		if(sb && !old_key[' '] && console_ptr - console_max - 1 != console_buffer)
		{
			*console_ptr++ = ' ';
			console_ptr[1] = 0;
		}
		old_key[' '] = sb;

		bool bb = glfwGetKey(GLFW_KEY_BACKSPACE);
	
		if(bb && !old_key[','] && console_ptr != console_buffer)
		{
			*--console_ptr = 0;
		}
		old_key[','] = bb;

		if(glfwGetKey(GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			keyb_input = true;
			if(console_ptr != console_buffer)
			{
				Packet* p = connection->packet_manager->start(NET_OP_CHAT_MESSAGE);
				
				p->writeString(console_buffer);

				connection->packet_manager->end();
			}
		}
	}
	else if(glfwGetKey('T'))
	{
		keyb_input = false;
		console_ptr = console_buffer;
		memset(console_buffer, 0, sizeof(console_buffer));
		old_key['T'] = true;
	}
}

void Client::update_connect()
{
	Address a; Packet p;

	while(net_socket->receive(a, p))
	{
		p.readHeader();
		connection->readSeq(p);

		switch(p.readChar())
		{
		case NET_OP_REQ_VC:
			if(p.readInt() == connection->key)
				connecting = false;
			break;		
		}

		delete [] p.data; p.data = NULL;
	}
}

void Client::draw_connect()
{
	//Load the viewport
	glViewport(0, 0, scw, sch);

	//Clear the buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	//Set projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, (GLdouble)scw / (GLdouble)sch, 0.1f, 1000.0f);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, -40.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	float time = (float)glfwGetTime() * 1.5f - 10.0f;

	using namespace glm;

	mat4 rot = mat4();

	glColor3f(1.0f, 1.0f, 1.0f);
	drawString("CONNECTING...", vec3(50.0f, 3.0f, -time),
												vec3(0.0f, -1.0f, 0.0f),
												transform3(rotate(mat4(), time, vec3(0.0f, 1.0f, 0.0f)), vec3(-1.0f, 0.0f, 0.0f)), 6.0f);
	
	glColor3f(0.7f, 0.7f, 0.7f);

	char buffer[ 30 ];

	Address* a = &connection->address;

	sprintf(buffer, "%d.%d.%d.%d:%d", a->ip_1, a->ip_2, a->ip_3, a->ip_4, a->ip_port); 

	drawString(buffer, vec3(48.0f, -4.0f, -time),
												vec3(0.0f, -0.7f, 0.0f),
												transform3(rotate(mat4(), time * 2, vec3(0.0f, 1.0f, 0.0f)), vec3(-1.0f, 0.0f, 0.0f)), 4.0f);


	glfwSwapBuffers();
}

void Client::update_dt()
{
	s_time = (float)glfwGetTime();
	dt = s_time - old_time;
	old_time = s_time;

	frame++;
}

void Client::update_game()
{

	if(new_load)
		request_scene();
	
	scene->update();

	process_packets();
	
	check_timeout();
}

void Client::process_packets()
{
	Address a; Packet p;
	while(net_socket->receive(a, p))
	{
		if(p.readHeader() && a == connection->address)
		{
			connection->readSeq(p);
			
			while(p.dptr - p.data < p.length)
			{
				switch(p.readChar())
				{

				case NET_OP_ADD_ENTITY:
					{
						int skip = p.readUint();
						int id = p.readInt();
						
						if(!scene->existsById(id))
						{
							Entity* e = Entity::create(connection, p);
							scene->add_static_id(e);
							e->id = id;
						}
						else
							p.dptr += skip;
							
					}
					break;

				case NET_OP_HEARTBEAT:
					break;

				case NET_OP_UPDATE_ENTITY:
					{
						int skip = p.readUint();
						Entity* e;
						if((e = scene->entityById(p.readInt())) && (e->last_update < connection->seq_num_remote))
						{
							e->receive(connection, p);
							e->last_update = connection->seq_num_remote;
						}
						else
							p.dptr += skip;
					}
					break;

				case NET_OP_CHAT_MESSAGE:

					char* c = p.readString();

					strcpy(console_recv_buffer + console_i * console_max, c);
					console_recv_age[console_i] = 8.0f;

					console_i = (console_i + 1) % console_recv_max;


					delete [] c;

					break;
				}
			}
		}

		delete [] p.data; p.data = NULL;
	}
}

void Client::check_timeout()
{
	 if(connection->last_recv > 10.0f)
	 {
		 running = false;
		 printf("Disconnected (timeout)");
	 }
}

void Client::request_scene()
{
	ReliablePacket p(PACKET_HEADERSZ_RELIABLE);
	
	p.writeHeader();
	connection->writeSeq(p);
	p.writeChar(NET_OP_REQ_SCENE);

	connection->sendReliablePacket(p);

	new_load = false;
}

void Client::draw_game()
{
	//Load the viewport
	glViewport(0, 0, scw, sch);

	//Clear the buffers
	glClearColor(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	//Set projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, (GLdouble)scw / (GLdouble)sch, 0.1f, 1000.0f);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	if(GLEW_EXT_texture_filter_anisotropic)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	scene->draw();


}

void Client::draw_console()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, scw, sch, 0, 0, 1);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(int i = 0; i < console_recv_max; i++)
	{
		int it = ((i + console_i) % console_recv_max);
		if(strlen(console_recv_buffer + it * console_max) > 0)
		{
			float s = 2.0f;
			glColor4f(1.0f, 1.0f, 1.0f, console_recv_age[it]);
			drawString2D(console_recv_buffer + it * console_max, vec2(10, sch - 10 - 20 * s - 7 * s * i), vec2(0, s), vec2(s, 0), 6);
		}
	}

	if(!keyb_input)
	{
		float s = 2.0f;
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		drawString2D(console_buffer, vec2(10, sch - 10 - 5 * s), vec2(0, s), vec2(s, 0), 6);
	}
	glDisable(GL_BLEND);
}