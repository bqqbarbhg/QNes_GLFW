#include <stdio.h>
#include <iostream>
#include <fstream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"
#include "binaryread.h"
#include "SOIL.h"

using namespace std;
using namespace glm;

Model::Model()
{
	
}


Model* Model::load(const char* name)
{
	Model* m = new Model;

	m->src = name;

	ifstream in(name, ios::binary);
	
	if(!in)
	{
		printf("Error! Model not found: %s", name);
		return m;
	}

	int tex_c;

	read_int(in, &tex_c);
	
	m->tex_ptr = new GLuint[tex_c];

	for(GLuint* u = m->tex_ptr; u < m->tex_ptr + tex_c; u++)
	{
		*u = SOIL_load_OGL_texture(
			read_string(in),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS  | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS
			);

	}

	read_int(in, &m->mesh_c);

	m->mesh_ptr = new Mesh[m->mesh_c];

	float* mat_buf = new float[16];
	

	for(Mesh* mesh = m->mesh_ptr; mesh < m->mesh_ptr + m->mesh_c; mesh++)
	{
		//Read the mesh name
		mesh->name = read_string(in);

		//Read the transformation matrix
		read_float(in, mat_buf, 16);
		mesh->transform = make_mat4(mat_buf);
		
		read_int(in, &mesh->part_c);
		mesh->part_ptr = new MeshPart[mesh->part_c];

		for(MeshPart* part = mesh->part_ptr; part < mesh->part_ptr + mesh->part_c; part++)
		{

			read_int(in, &part->elem_c);
			part->elem_ptr = new VertexElement[part->elem_c];

			read_int(in, &part->vert_stride);

			for(VertexElement* elem = part->elem_ptr; elem < part->elem_ptr + part->elem_c; elem++)
			{
				in.read(&elem->usage, 1);
				read_int(in, &elem->index);
				read_int(in, &elem->offset);
			}

			//Read the vertices
			read_int(in, &part->vert_c);
			part->vert_ptr = new float[part->vert_c];
		
			read_float(in, part->vert_ptr, part->vert_c);
		
			//Read the indices
			read_int(in, &part->index_c);

			int bits_per_index;
			read_int(in, &bits_per_index);

			if(bits_per_index == 16)
			{
				part->index_16 = true;
				part->index_ptr_16 = new unsigned short[part->index_c];
			
				read_unsigned_short(in, part->index_ptr_16, part->index_c);
			}
			else if(bits_per_index == 32)
			{
				part->index_16 = false;
				part->index_ptr_32 = new unsigned int[part->index_c];

				read_unsigned_int(in, part->index_ptr_32, part->index_c);
			}
			else
			{
				printf("%s: Invalid amount of bits per index! %d: expected 16 or 32", name, bits_per_index);
			}
		
			//Generate the buffers
			part->createBuffers();

			//Read the textures
			read_int(in, &part->tex_c);
			
			part->tex_ptr = new GLuint[part->tex_c];
			
			int t;
			for(GLuint* u = part->tex_ptr; u < part->tex_ptr + part->tex_c; u++)
			{
				read_int(in, &t);
				*u = m->tex_ptr[t]; 
			}
		}
	}

	free(mat_buf);
	return m;
}

Model* Model::loadNoTex(const char* name)
{
	Model* m = new Model;

	m->src = name;

	ifstream in(name, ios::binary);
	
	if(!in)
	{
		printf("Error! Model not found: %s", name);
		return m;
	}

	int tex_c;

	read_int(in, &tex_c);
	
	m->tex_ptr = new GLuint[tex_c];

	for(int i = 0; i < tex_c; i++)
	{
		read_string(in);
	}

	read_int(in, &m->mesh_c);

	m->mesh_ptr = new Mesh[m->mesh_c];

	float* mat_buf = new float[16];
	

	for(Mesh* mesh = m->mesh_ptr; mesh < m->mesh_ptr + m->mesh_c; mesh++)
	{
		//Read the mesh name
		mesh->name = read_string(in);

		//Read the transformation matrix
		read_float(in, mat_buf, 16);
		mesh->transform = make_mat4(mat_buf);
		
		read_int(in, &mesh->part_c);
		mesh->part_ptr = new MeshPart[mesh->part_c];

		for(MeshPart* part = mesh->part_ptr; part < mesh->part_ptr + mesh->part_c; part++)
		{

			read_int(in, &part->elem_c);
			part->elem_ptr = new VertexElement[part->elem_c];

			read_int(in, &part->vert_stride);

			for(VertexElement* elem = part->elem_ptr; elem < part->elem_ptr + part->elem_c; elem++)
			{
				in.read(&elem->usage, 1);
				read_int(in, &elem->index);
				read_int(in, &elem->offset);
			}

			//Read the vertices
			read_int(in, &part->vert_c);
			part->vert_ptr = new float[part->vert_c];
		
			read_float(in, part->vert_ptr, part->vert_c);
		
			//Read the indices
			read_int(in, &part->index_c);

			int bits_per_index;
			read_int(in, &bits_per_index);

			if(bits_per_index == 16)
			{
				part->index_16 = true;
				part->index_ptr_16 = new unsigned short[part->index_c];
			
				read_unsigned_short(in, part->index_ptr_16, part->index_c);
			}
			else if(bits_per_index == 32)
			{
				part->index_16 = false;
				part->index_ptr_32 = new unsigned int[part->index_c];

				read_unsigned_int(in, part->index_ptr_32, part->index_c);
			}
			else
			{
				printf("%s: Invalid amount of bits per index! %d: expected 16 or 32", name, bits_per_index);
			}
		
			//Generate the buffers
			//part->createBuffers();

			//Read the textures
			read_int(in, &part->tex_c);
			
			part->tex_ptr = new GLuint[part->tex_c];
			
			int t;
			for(GLuint* u = part->tex_ptr; u < part->tex_ptr + part->tex_c; u++)
			{
				read_int(in, &t);
				*u = m->tex_ptr[t]; 
			}
		}
	}

	free(mat_buf);
	return m;
}

void Model::draw()
{
	for(Mesh* mesh = mesh_ptr; mesh < mesh_ptr + mesh_c; mesh++)
	{
		mesh->draw();
	}
}

void Model::draw(mat4 t)
{
	for(Mesh* mesh = mesh_ptr; mesh < mesh_ptr + mesh_c; mesh++)
	{
		mesh->draw(t);
	}
}
