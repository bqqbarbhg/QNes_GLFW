#include "textrender.h"
#include <GL/glew.h>
#include <GL/glfw.h>

#include "glm/gtc/type_ptr.hpp"

int* font;

using namespace glm;

void initTextRender()
{
	font = new int[0x100];

	font['A'] = 0x1f294bf;
	font['B'] = 0xaad6bf;
	font['C'] = 0x118c63f;
	font['D'] = 0xe8c63f;
	font['E'] = 0x118d6bf;
	font['F'] = 0x1094bf;
	font['G'] = 0x1dad63f;
	font['H'] = 0x1f2109f;
	font['I'] = 0x118fe31;
	font['J'] = 0x1f84218;
	font['K'] = 0x115109f;
	font['L'] = 0x108421f;
	font['M'] = 0x1f1105f;
	font['N'] = 0x1f4105f;
	font['O'] = 0x1f8c63f;
	font['P'] = 0x7294bf;
	font['Q'] = 0x174d63f;
	font['R'] = 0x17694bf;
	font['S'] = 0x1dad6b7;
	font['T'] = 0x10fc21;
	font['U'] = 0x1f8421f;
	font['V'] = 0x364183;
	font['W'] = 0xf8220f;
	font['X'] = 0x1151151;
	font['Y'] = 0x117041;
	font['Z'] = 0x119d731;
	font['0'] = 0xfc7e0;
	font['1'] = 0x7c40;
	font['2'] = 0x9d720;
	font['3'] = 0xfd620;
	font['4'] = 0xf90e0;
	font['5'] = 0x4d6e0;
	font['6'] = 0xed7e0;
	font['7'] = 0xf9420;
	font['8'] = 0xfd7e0;
	font['9'] = 0xf94e0;
	font['.'] = 0x4000;
	font['!'] = 0x5c00;
	font['?'] = 0x3d420;
	font['+'] = 0x21080;
	font['_'] = 0x1084210;
	font['~'] = 0x441044;
	font['*'] = 0x288a0;
	font['+'] = 0x23880;
	font['&'] = 0x105d760;
	font['/'] = 0x1041041;
	font['\\'] = 0x111110;
	font['|'] = 0x7c00;
	font['^'] = 0x10440;
	font['<'] = 0x2880;
	font['>'] = 0x22800;
	font['='] = 0x52940;
	font['\''] = 0x18060;
	font['\"'] = 0xc00;
	font[':'] = 0x2800;
	font[' '] = 0x0;
}

void drawChar(char c, vec3 p, vec3 d, vec3 r)
{
	if(font[c])
	{
		for(int x = 0; x < 5; x++)
		{
			for(int y = 0; y < 5; y++)
			{
				if(font[c] & (1 << (x * 5 + y)))
				{
					float xf = (float)x;
					float yf = (float)y;
					glVertex3fv(value_ptr(p + xf * r + yf * d));
					glVertex3fv(value_ptr(p + (xf + 1.0f) * r + yf * d));
					glVertex3fv(value_ptr(p + (xf + 1.0f) * r + (yf + 1.0f) * d));
					glVertex3fv(value_ptr(p + xf * r + (yf + 1.0f) * d));
				}
			}
		}
	}
}

void drawString(char* s, vec3 p, vec3 d, vec3 r, float c)
{
	glBegin(GL_QUADS);

	float f = 0;
	do
	{
		drawChar(*s, p + f * r, d, r);
		f += c;
		s++;
	} while (*s != 0);

	glEnd();
}

/*
function drawChar(x,y,size,chr){
	for(var i=0;i<5;i++){
		for(var j=0;j<5;j++){
			if(Font[chr] && Font[chr] & (1 << (i*5+j))){
				drawRect(x+i*size,y+j*size,size,size);
			}
		}
	}
}

function drawString(x,y,size,str){
	str = str.toUpperCase();
	var l = str.length;
	for(var i=0;i<l;i++){
		chr = str.charAt(i);
		drawChar(x + i*size*6,y,size,chr);
	}
}
*/