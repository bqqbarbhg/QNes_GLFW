#pragma once

#include "glm/glm.hpp"

#define NET_SAFE

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif


#if PLATFORM == PLATFORM_WINDOWS

    #include <winsock2.h>
	
	#pragma comment( lib, "wsock32.lib" )

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>

#endif

const unsigned int MAX_PACKET_SIZE = 2048;
const unsigned int PACKET_HEADERSZ_GENERAL	=	sizeof(char) * 4	//Protocol ID
											+	sizeof(char) * 1;	//Operation

const unsigned int PACKET_HEADERSZ_SEQ = sizeof(int) * 3;

const unsigned int PACKET_HEADERSZ_RELIABLE = PACKET_HEADERSZ_GENERAL + PACKET_HEADERSZ_SEQ;

const unsigned int PACKET_HEADERSZ_WRITE =			sizeof(char) //Entity Type
												+	sizeof(int); //Entity ID

const unsigned int PACKET_HEADERSZ_ENTITY = PACKET_HEADERSZ_GENERAL + sizeof(int);
const unsigned int PACKET_HEADER_PROC =		((unsigned int)('Q') << sizeof(char)*8*0)
										|	((unsigned int)('N') << sizeof(char)*8*1)
										|	((unsigned int)('e') << sizeof(char)*8*2)
										|	((unsigned int)('s') << sizeof(char)*8*3);

bool initSockets();

void terminateSockets();


const char PACKET_STATE_LOST = 2;
const char PACKET_STATE_ACKED = 1;
const char PACKET_STATE_PENDING = 0;

const char NET_OP_REQ_VC				= 0;
const char NET_OP_REQ_SCENE				= 1;
const char NET_OP_ADD_ENTITY			= 2;
const char NET_OP_HEARTBEAT				= 3;
const char NET_OP_UPDATE_ENTITY			= 4;
const char NET_OP_CHAT_MESSAGE			= 5;
const char NET_OP_SEND_INPUT			= 6;


class Address
{
public:
	Address();
	Address(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short port);

	void packIP();
	void unpackIP();

	unsigned int ip_address;
	unsigned int ip_1, ip_2, ip_3, ip_4;
	unsigned short ip_port;

	sockaddr_in socket_address;

	bool operator == ( const Address & rhs) const;
	bool operator != ( const Address & rhs) const;
};

class Packet
{
public:
	Packet();
	Packet(int len);
	Packet(int len, char* d);
	~Packet();

	char* data;
	char* dptr;
	int length;

	void send(const Address& address);

	void writeChar(const char c);
	void writeInt(const int i);
	void writeUint(const unsigned int i);
	void writeFloat(const float& f);
	void writeVec3(const glm::vec3& v);
	void writeMat4(const glm::mat4& m);
	void writeString(const char* str);
	void writeHeader();

	char readChar();
	int readInt();
	unsigned int readUint();
	float readFloat();
	glm::vec3 readVec3();
	glm::mat4 readMat4();
	char* readString();
	bool readHeader();
};

class ReliablePacket : public Packet
{
public:
	ReliablePacket();
	ReliablePacket(int len);
	ReliablePacket(int len, char* d);

	void reset();
	int seq_num;
	float age;
};


class Socket
{
public:
	Socket();

	bool open(unsigned short port);

	void close();

	bool send(const Address& address, const Packet& packet);
	bool send(const Address& address, const void* packet_data, const int packet_size);
	bool send(const sockaddr_in& address, const char* packet_data, const int packet_size);

	int receive(Address& sender, char* buffer);
	bool receive(Address& sender, Packet& packet);

private:
	int handle;
};


extern Socket* net_socket;
