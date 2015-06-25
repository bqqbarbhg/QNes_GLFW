#if(0)

#ifndef _DEBUG
#define GLEW_STATIC
#endif

#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"
#include "SOIL.h"
#include "Scene.h"
#include "EntityModelStatic.h"
#include "EntityFreeCamera.h"
#include "EntityPlayerControlProto.h"
#include "Camera.h"
#include "network.h"
#include "stringop.h"

using namespace std;
using namespace glm;
using namespace gtc;

Camera* camera = new Camera();
Scene* scene = new Scene();
float dt;

Socket* net_socket = new Socket();

void update();
void draw();
void net_update();
void initGlfwGlew();

bool running = true;
float old_time = 0.0f;

int scw = 1920, sch = 1080;

bool test_rec = false;
	Address a;

	GLfloat lambient[] = {0.05f, 0.05f, 0.8f, 1.0f};
	GLfloat ldiffuse[] = {1.0f, 1.0f, 0.8f, 1.0f};
	GLfloat lposition[]= { 4.0f, 6.0f, -4.0f, 0.0f };
	GLfloat global_ambient[] = { 0.0f, 0.0f, 0.3f, 1.0f };

void initGlfwGlew()
{
	glfwInit();
	
	if(!glfwOpenWindow(scw, sch, 0, 0, 0, 0, 26, 8, GLFW_FRAMELESS))
	{
		glfwTerminate();
	}


	glewInit();
	glfwSetWindowTitle("Q N.E.S");
}

int main(int argc, char** argv)
{
	printf("O HAI MARK\n");
	
	if(!initSockets())
		printf("::(");



	char* line = new char[100];
	bool inc = true;

	while(inc)
	{
		cin.getline(line, 100);

		if( strcmpl(line, "scw") )
			scw = atoi(line + 4);
		if( strcmpl(line, "sch") )
			scw = atoi(line + 4);

		if( strcmpl(line, "socket ") )
		{
			int c_port;
			c_port = atoi(line + 7);
			printf("Opening socket to port %d...\n", c_port);

			if(!net_socket->open(c_port))
			{
				printf("Failed to open socket\n");
				//printf("Error: %d\n", WSAGetLastError);
			}
			else
			{
				printf("Socket opened succesfully!\n");
			}
		}

		if( strcmpl(line, "client "))
		{
			strtok(line, " ");
			Address m = Address(	(unsigned int)atoi(strtok(NULL, ".")),
									(unsigned int)atoi(strtok(NULL, ".")),
									(unsigned int)atoi(strtok(NULL, ".")),
									(unsigned int)atoi(strtok(NULL, ".")),
									(unsigned int)atoi(strtok(NULL, ".")));

			a = Address(m.ip_port, m.ip_4, m.ip_3, m.ip_2, m.ip_1);

			printf("Connecting to %d.%d.%d.%d:%d\n", a.ip_1, a.ip_2, a.ip_3, a.ip_4, a.ip_port);
			test_rec = false;
		}

		if( !strcmp(line, "c"))
		{
			net_socket->open(0);
			a = Address(127, 0, 0, 1, 30000);
			test_rec = false;
			inc = false;
		}
		if(!strcmp(line, "s"))
		{
			net_socket->open(30000);
			test_rec = true;
			inc = false;
		}
		if( !strcmp(line, "server") )
		{
			test_rec = true;
		}

		if(strcmp(line, "run") == 0)
			inc = false;

	}
	delete [] line;

	initGlfwGlew();

	
	scene->add(new EntityModelStatic(Model::load("tscene.omf")));
	scene->add(new EntityPlayerControlProto(vec3(0, 5, 0)));

	while (running)
	{

		net_update();

		update();

		draw();

		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			printf("OpenGL error: %s!\n", (char*)gluErrorString(error));
		}

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam( GLFW_OPENED);
	}



	glfwTerminate();

}

void net_update()
{
	if(test_rec)
		scene->receive();
	else
		scene->send(a);
}


void update()
{
	float time = glfwGetTime();
	dt = time - old_time;
	old_time = time;

	char* b = new char[30];
	itoa((int)(1.0f / dt), b, 10);

	glfwSetWindowTitle(b);

	scene->update();

}


void draw()
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
	vec3 up(0, 1, 0);
	
	//Set view
	

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, lposition);


		glEnable(GL_NORMALIZE);

		glEnable(GL_TEXTURE_2D);

		glEnable(GL_DEPTH_TEST);

		float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
	// now, draw polygon as its material properties will be affected by the glMaterialfv call.

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	//gluLookAt(0.0f, 100.0f, -200.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	camera->apply();
	
	scene->draw();

	

	glfwSwapBuffers();

}

#endif