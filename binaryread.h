#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>

void read_int(std::ifstream& strm, int* i)
{ strm.read(reinterpret_cast<char*>(i), sizeof(int));}

void read_int(std::ifstream& strm, int* i, int c)
{ strm.read(reinterpret_cast<char*>(i), sizeof(int) * c);}


void read_unsigned_int(std::ifstream& strm, unsigned int* i)
{ strm.read(reinterpret_cast<char*>(i), sizeof(int));}

void read_unsigned_int(std::ifstream& strm, unsigned int* i, int c)
{ strm.read(reinterpret_cast<char*>(i), sizeof(int) * c);}


void read_float(std::ifstream& strm, float* f)
{ strm.read(reinterpret_cast<char*>(f), sizeof(float)); }

void read_float(std::ifstream& strm, float* f, int c)
{ strm.read(reinterpret_cast<char*>(f), sizeof(float) * c);}


void read_short(std::ifstream& strm, short* i)
{ strm.read(reinterpret_cast<char*>(i), sizeof(short));}

void read_short(std::ifstream& strm, short* i, int c)
{ strm.read(reinterpret_cast<char*>(i), sizeof(short) * c);}


void read_unsigned_short(std::ifstream& strm, unsigned short* i)
{ strm.read(reinterpret_cast<char*>(i), sizeof(short));}

void read_unsigned_short(std::ifstream& strm,  unsigned short* i, int c)
{ strm.read(reinterpret_cast<char*>(i), sizeof(short) * c);}


bool read_fail(std::ifstream& strm, char* message)
{ if(strm.fail()) { printf(message); printf("\nBytes read: %d\n", strm.gcount()); return strm.fail(); } return false; }



int read_7bit_encoded_int(std::ifstream& in)
{
	char a;
	int b = 0;
	int c = 0;
	do
	{
		if(c == 0x23)
		{
			printf("Error parsing 7-bit encoded int!");
			return -1;
		}
		in.read(&a, 1);
		b |= (a & 0x7f) << c;
		c += 7;
	}
	while((a&0x80) != 0);

	return b;
}

char* read_string(std::ifstream& in)
{
	int len = read_7bit_encoded_int(in);
	char* buf = new char[len + 1];
	buf[len] = 0;
	in.read(buf, len);
	return buf;
}