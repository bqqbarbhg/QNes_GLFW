#include "virtualconnection.h"
#include "Entity.h"

class Server
{
public:
	Server(int max_players);

	int conn_c;
	VirtualConnection** conn_ptr;

	void run();
	bool running;

	VirtualConnection* getVC(Address a);
	int getID(Address a);

private:
	void addEntityCallback(Entity* e);
	void receive_packets();
	void send_packets();
	void update_dt();
	void update_connections();
	float old_time;
};