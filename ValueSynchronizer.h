#include "network.h"

const int SYNC_TYPE_CHAR = 0;
const int SYNC_TYPE_INT = 1;
const int SYNC_TYPE_FLOAT = 2;
const int SYNC_TYPE_VEC3 = 3;


class ValueSynchronizer
{
private:
	
	struct SyncValue
	{
		SyncValue(int type, void* var_ptr, int sync_freq, int sync_freq_static, int sync_freq_decay);

		void *	val_ptr;
		void *	val_old_ptr;
		bool	dyn_sync_freq;
		bool	need_sync;
		int		type;
		int		freq;
		int		freq_static;
		int		dyn_sync_decay;
		int		dyn_sync_decay_current;

		inline int		getSize();
		inline bool		isDirty();
		inline void		updateOld();
		inline void		send(Packet& p);
		inline void		receive(Packet& p);

	}			*sync_ptr;
	int			sync_c;
public:

	ValueSynchronizer();

	void add(int type, void* var_ptr, int sync_freq);
	void add(int type, void* var_ptr, int sync_freq, int sync_freq_static, int sync_freq_decay);

	void send(Packet& p);
	void receive(Packet& p);
	void updateSyncList();
	int getSize();

	int time;

};