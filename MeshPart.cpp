#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include "MeshPart.h"

MeshPart::MeshPart()
{

}

void MeshPart::createBuffers()
{
	glGenBuffersARB(1, &vbo_vert);
	glBindBufferARB(GL_ARRAY_BUFFER, vbo_vert);
	glBufferDataARB(GL_ARRAY_BUFFER, vert_c * sizeof(float), vert_ptr, GL_STATIC_DRAW_ARB);
	
	glGenBuffersARB(1, &vbo_index);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, index_c * (index_16 ? sizeof(short) : sizeof(int)), index_16 ? (void*)index_ptr_16 : (void*)index_ptr_32, GL_STATIC_DRAW_ARB);

	/*delete [] vert_ptr;
	if(index_16)
		delete [] index_ptr_16;
	else
		delete [] index_ptr_32;*/
}

VertexElement* MeshPart::getElement(char c, int i)
{
	for(VertexElement* v = elem_ptr; v < elem_ptr + elem_c; v++)
	{
		if(v->usage == c && v->index == i)
			return v;
	}
	return NULL;
	
}

void MeshPart::draw()
{
		glBindTexture(GL_TEXTURE_2D, *tex_ptr);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_vert);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_index);

	for(VertexElement* elem = elem_ptr; elem < elem_ptr + elem_c; elem++)
	{
		switch (elem->usage)
		{
		case VERTEX_ELEMENT_USAGE_POSITION:
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, vert_stride, (const GLubyte *)0 + elem->offset);

			break;
		case VERTEX_ELEMENT_USAGE_NORMAL:
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, vert_stride, (const GLubyte *)0 + elem->offset);

			break;
		case VERTEX_ELEMENT_USAGE_TEXTURE:
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, vert_stride, (const GLubyte *)0 + elem->offset);

			break;
		}
	}

	glDrawElements(GL_TRIANGLES, index_c, index_16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
}