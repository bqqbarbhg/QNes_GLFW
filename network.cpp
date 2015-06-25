#include <stdio.h>
#include "network.h"
#include "glm/gtc/type_ptr.hpp"
#include "virtualconnection.h"
//General

bool initSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2), &WsaData ) == NO_ERROR;
    #else
    return true;
    #endif
}

void terminateSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}




//Socket

Socket::Socket()
{

}

bool Socket::open(unsigned short port)
{
	handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(handle <= 0)
	{
		printf("Couldn't create socket.\n", port);
		return false;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short) port);

	if(bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0)
	{
		printf("Failed to bind socket, port: %d\n", port);

#if PLATFORM == PLATFORM_WINDOWS
		printf("Error: %d", WSAGetLastError());
#endif

		close();
		return false;
	}

	#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

    int nonBlocking = 1;
    if ( fcntl( handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
        printf( "Failed to set non-blocking socket.\n" );
		close();
        return false;
    }

    #elif PLATFORM == PLATFORM_WINDOWS

        DWORD nonBlocking = 1;
        if ( ioctlsocket( handle, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "Failed to set non-blocking socket.\n" );
			close();
            return false;
        }

    #endif

	return true;
}

bool Socket::send(const Address& address, const Packet& packet)
{ return this->send(address.socket_address, packet.data, packet.length);}

bool Socket::send(const Address& address, const void* packet_data, const int packet_size)
{ return this->send(address.socket_address,  (const char*)packet_data, packet_size);}

bool Socket::send(const sockaddr_in& address, const char* packet_data, const int packet_size)
{
	//if (not work) {'''    reinterpret_cast<const char*>(packet_data)  '''}
	int bytes_sent = sendto(handle, packet_data, packet_size,
							0, (sockaddr*)&address, sizeof(sockaddr_in));
	
	if(bytes_sent != packet_size)
	{
		printf("Failed to send packet. Return value: %d\n", bytes_sent);

#if PLATFORM == PLATFORM_WINDOWS
		printf("Error code %d\n", WSAGetLastError());
#endif
		return false;
	}
	return true;
}

int Socket::receive( Address  &sender, char* buffer)
{
	#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

	sockaddr_in from;
	socklen_t fromLen = sizeof(from);

	int bytes_received = recvfrom(handle, buffer, MAX_PACKET_SIZE, 
								0, (sockaddr*)&from, &fromLen);

	if(bytes_received <= 0)
		return -1;

	sender.socket_address = from;
	sender.ip_address = ntohl(from.sin_addr.s_addr);
	sender.ip_port = ntohs(from.sin_port);
	sender.unpackIP();

	return bytes_received;
}

bool Socket::receive(Address &sender, Packet &packet)
{
	char* buf = new char[MAX_PACKET_SIZE];
	int len = receive(sender, buf);
	if(len == -1)
	{
		delete [] buf;
		return false;
	}
	packet.length = len;
	packet.data = buf;
	packet.dptr = packet.data;
	return true;
}

void Socket::close()
{
	#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close( handle );
    #elif PLATFORM == PLATFORM_WINDOWS
    closesocket( handle );
    #endif
}




//Address

Address::Address(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short port)
{
	unsigned int packed_ip_addr = (a << 24) | (b << 16) | (c << 8) | d;

	ip_1 = a; ip_2 = b; ip_3 = c; ip_4 = d;
	ip_port = port;

	socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl( packed_ip_addr );
    socket_address.sin_port = htons( port );
}

void Address::unpackIP()
{
	ip_1 = (ip_address >> 24) & 0xff;
	ip_2 = (ip_address >> 16) & 0xff;
	ip_3 = (ip_address >> 8) & 0xff;
	ip_4 = ip_address & 0xff;
}

void Address::packIP()
{
	unsigned int packed_ip_addr = (ip_1 << 24) | (ip_2 << 16) | (ip_3 << 8) | ip_4;

	socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl( packed_ip_addr );
    socket_address.sin_port = htons( ip_port );
}

Address::Address()
{

}

bool Address::operator== (const Address& rhs) const
{
	return rhs.ip_1 == ip_1 && rhs.ip_2 == ip_2 && rhs.ip_3 == ip_3 && rhs.ip_4 == ip_4 && rhs.ip_port == ip_port;
}

bool Address::operator!= (const Address& rhs) const
{
	return !(rhs == *this);
}


//Packet

Packet::Packet()
{
	data = NULL;
}

Packet::Packet(int len)
{
	length = len;
	data = new char[len];
	dptr = data;
}

Packet::Packet(int len, char* d)
{
	length = len;
	data = d;
	dptr = data;
}

Packet::~Packet()
{
	if(data != NULL)
		delete [] data;
}

void Packet::send(const Address& a)
{
	net_socket->send(a, *this);
}

void Packet::writeChar(const char c)
{
	*dptr= c;
	dptr++;
}

void Packet::writeInt(const int i)
{
	*reinterpret_cast<int*>(dptr) = i;
	dptr += sizeof(int);
}

void Packet::writeUint(const unsigned int i)
{
	*reinterpret_cast<unsigned int*>(dptr) = i;
	dptr += sizeof(unsigned int);
}

void Packet::writeFloat(const float& f)
{
	*reinterpret_cast<float*>(dptr) = f;
	dptr += sizeof(float);
}

void Packet::writeVec3(const glm::vec3& v)
{
	writeFloat(v.x);
	writeFloat(v.y);
	writeFloat(v.z);
}

void Packet::writeMat4(const glm::mat4& m)
{
	memcpy(dptr, glm::value_ptr(m), sizeof(glm::mat4));
	dptr += sizeof(glm::mat4);
}

void Packet::writeString(const char* s)
{
	int sz = strlen(s) + 1;
	memcpy(dptr, s, sz);
	dptr += sz;
}

void Packet::writeHeader()
{
	writeInt(PACKET_HEADER_PROC);
}

char Packet::readChar()
{
	return *dptr++;
}

int Packet::readInt()
{
	int i = *reinterpret_cast<int*>(dptr);
	dptr += sizeof(int);
	return i;
}

unsigned int Packet::readUint()
{
	int i = *reinterpret_cast<unsigned int*>(dptr);
	dptr += sizeof(unsigned int);
	return i;
}

float Packet::readFloat()
{
	float f = *reinterpret_cast<float*>(dptr);
	dptr += sizeof(float);
	return f;
}

glm::vec3 Packet::readVec3()
{
	float x, y, z;
	x = readFloat();
	y = readFloat();
	z = readFloat();
	return glm::vec3(x, y, z);
}

glm::mat4 Packet::readMat4()
{
	glm::mat4 m = glm::make_mat4(reinterpret_cast<float*>(dptr));
	dptr += sizeof(glm::mat4);
	return m;
}

char* Packet::readString()
{
	int len = strlen(dptr) + 1;
	char* c = (char*)memcpy(malloc(len), dptr, len);
	dptr += len;
	return c;
}

bool Packet::readHeader()
{
	return readInt() == PACKET_HEADER_PROC;
}



//ReliablePacket

ReliablePacket::ReliablePacket()
{
	data = NULL;
	age = 0.0f;
}

ReliablePacket::ReliablePacket(int len)
{
	length = len;
	data = new char[len];
	dptr = data;
	age = 0.0f;
}

ReliablePacket::ReliablePacket(int len, char* d)
{
	length = len;
	data = d;
	dptr = data;
	age = 0.0f;
}

void ReliablePacket::reset()
{
	dptr = data;
}
