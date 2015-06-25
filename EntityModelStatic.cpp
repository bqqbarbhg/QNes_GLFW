#pragma once

#include <GL/glew.h>
#include <GL/glfw.h>
#include "EntityModelStatic.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "transform3.h"
#include "Scene.h"

using namespace glm;

EntityModelStatic::EntityModelStatic(Model* m)
{
	model = m;
	src = m->src;
	transform = mat4();
	transform = scale(transform, vec3(0.2f));
}

void EntityModelStatic::draw()
{
	model->draw(transform);
}

void EntityModelStatic::init()
{
	StaticCollisionMesh** smcptr = scene->expandStaticCollision(model->mesh_c);
	
	for(Mesh* mesh = model->mesh_ptr; mesh < model->mesh_ptr + model->mesh_c; mesh++)
	{
		int vertsize = 0;
		int indexsize = 0;
		for(MeshPart* part = mesh->part_ptr; part < mesh->part_ptr + mesh->part_c; part++)
		{
			vertsize += part->vert_c / (part->vert_stride / sizeof(float));
			indexsize += part->index_c;
		}
		vec3* verts = new vec3[vertsize];
		unsigned int* indices = new unsigned int[indexsize];
		
		vec3* vptr = verts;
		unsigned int* iptr = indices;

		for(MeshPart* part = mesh->part_ptr; part < mesh->part_ptr + mesh->part_c; part++)
		{
			int stride = part->vert_stride;
			int offset = part->getElement(VERTEX_ELEMENT_USAGE_POSITION, 0)->offset;

			mat4 xform = transform * mesh->transform;

			for(float* f = part->vert_ptr + offset; f < part->vert_ptr + part->vert_c; f += stride / sizeof(float))
			{
				*vptr++ = transform3(xform, vec3(f[0], f[1], f[2]));
			}

			if(part->index_16)
			{
				for(unsigned short* s = part->index_ptr_16; s < part->index_ptr_16 + part->index_c; s++)
				{
					*iptr++ = (unsigned int)*s;
				}
			}
			else
			{
				for(unsigned int* i = part->index_ptr_32; i < part->index_ptr_32 + part->index_c; i++)
				{
					*iptr++ = *i;
				}
			}
		}

		*smcptr++ = new StaticCollisionMesh(indices, verts, indexsize);

	}
}

void EntityModelStatic::write(VirtualConnection* v)
{
	Packet* p = v->packet_manager->startDlen(NET_OP_ADD_ENTITY);

	p->writeInt(id);
	p->writeChar(ENTITY_TYPE_ENTITY_MODEL_STATIC);
	p->writeMat4(transform);
	p->writeString(model->src);

	v->packet_manager->endDlen();
}

EntityModelStatic* EntityModelStatic::create(VirtualConnection* v, Packet& p)
{
	mat4 transform = p.readMat4();
	char* buf = p.readString();

	EntityModelStatic* ems = new EntityModelStatic(Model::load(buf));
	ems->transform = transform;

	return ems;
}