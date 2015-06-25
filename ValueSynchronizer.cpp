//REMOVE E AEHNFU
//TODO: if not

#include <stdio.h>

#include "ValueSynchronizer.h"

ValueSynchronizer::SyncValue::SyncValue(int t, void* ptr, int fr, int sfr, int dsc)
{
	type = t;
	val_ptr = ptr;
	freq = fr;
	freq_static = sfr;
	dyn_sync_decay = dsc;
	dyn_sync_decay_current = 0;

	if(dyn_sync_freq = freq_static != -1)
		val_old_ptr = malloc(getSize());
}

inline int ValueSynchronizer::SyncValue::getSize()
{
	switch(type)
	{
		case SYNC_TYPE_CHAR:
			return sizeof(char);		break;
		case SYNC_TYPE_INT:
			return sizeof(int);			break;
		case SYNC_TYPE_FLOAT:
			return sizeof(float);		break;
		case SYNC_TYPE_VEC3:
			return sizeof(glm::vec3);	break;
	}
}

inline bool ValueSynchronizer::SyncValue::isDirty()
{
	switch(type)
	{
		case SYNC_TYPE_CHAR:
			return *reinterpret_cast<char*>(val_ptr) != *reinterpret_cast<char*>(val_old_ptr);
		case SYNC_TYPE_INT:
			return *reinterpret_cast<int*>(val_ptr) != *reinterpret_cast<int*>(val_old_ptr);
		case SYNC_TYPE_FLOAT:
			return *reinterpret_cast<float*>(val_ptr) != *reinterpret_cast<float*>(val_old_ptr);
		case SYNC_TYPE_VEC3:
			return *reinterpret_cast<glm::vec3*>(val_ptr) != *reinterpret_cast<glm::vec3*>(val_old_ptr);
	}
}

inline void ValueSynchronizer::SyncValue::updateOld()
{
	if(isDirty())
		dyn_sync_decay_current = dyn_sync_decay;
	dyn_sync_decay_current--;

	switch(type)
	{
		case SYNC_TYPE_CHAR:
			*reinterpret_cast<char*>(val_old_ptr) = *reinterpret_cast<char*>(val_ptr);				break;
		case SYNC_TYPE_INT:
			*reinterpret_cast<int*>(val_old_ptr) = *reinterpret_cast<int*>(val_ptr);				break;
		case SYNC_TYPE_FLOAT:
			*reinterpret_cast<float*>(val_old_ptr) = *reinterpret_cast<float*>(val_ptr);			break;
		case SYNC_TYPE_VEC3:
			*reinterpret_cast<glm::vec3*>(val_old_ptr) = *reinterpret_cast<glm::vec3*>(val_ptr);	break;
	}
}

inline void ValueSynchronizer::SyncValue::send(Packet& p)
{
	switch(type)
	{
		case SYNC_TYPE_CHAR:
			 p.writeChar(*reinterpret_cast<char*>(val_ptr));		break;
		case SYNC_TYPE_INT:
			 p.writeInt(*reinterpret_cast<int*>(val_ptr));			break;
		case SYNC_TYPE_FLOAT:
			 p.writeFloat(*reinterpret_cast<float*>(val_ptr));		break;
		case SYNC_TYPE_VEC3:
			 p.writeVec3(*reinterpret_cast<glm::vec3*>(val_ptr));	break;
	}
}

inline void ValueSynchronizer::SyncValue::receive(Packet& p)
{
	switch(type)
	{
		case SYNC_TYPE_CHAR:
			*reinterpret_cast<char*>(val_ptr) = p.readChar();			break;
		case SYNC_TYPE_INT:
			*reinterpret_cast<int*>(val_ptr) = p.readInt();				break;
		case SYNC_TYPE_FLOAT:
			*reinterpret_cast<float*>(val_ptr) = p.readFloat();			break;
		case SYNC_TYPE_VEC3:
			*reinterpret_cast<glm::vec3*>(val_ptr) = p.readVec3();		break;
	}
}

ValueSynchronizer::ValueSynchronizer()
{
	sync_c = 0;
	time = 0;
}

void ValueSynchronizer::add(int t, void* ptr, int fr)
{
	add(t, ptr, fr, -1, 0);
}

void ValueSynchronizer::add(int t, void* ptr, int fr, int sfr, int dsc)
{
	if(sync_c != 0)
	{
		SyncValue* old = sync_ptr;
		sync_ptr = (ValueSynchronizer::SyncValue*)malloc(sizeof(ValueSynchronizer::SyncValue) * (sync_c + 1));
		memcpy(sync_ptr, old, sizeof(ValueSynchronizer::SyncValue) *  sync_c);
		free(old);
	}
	else
	{
		sync_ptr = (ValueSynchronizer::SyncValue*)malloc(sizeof(ValueSynchronizer::SyncValue));
	}

	sync_ptr[sync_c] = ValueSynchronizer::SyncValue(t, ptr, fr, sfr, dsc);

	sync_c++;
}

void ValueSynchronizer::send(Packet& p)
{
	SyncValue* sync_end = sync_ptr + sync_c;
	
	unsigned char c = 0;
	unsigned char i = 0;

	for (SyncValue *v = sync_ptr; v != sync_end; v++)
	{
		c >>= 1;

		if(v->need_sync)
			c |= 0x80;
		
		if(++i == 8)
		{
			p.writeChar(c);

			for(SyncValue *v2 = v - 7; v2 != v + 1; v2++)
			{
				if(v2->need_sync)
					v2->send(p);

				if(v->dyn_sync_freq)
					v->updateOld();
			}

			i = 0;
		}
	}

	if(i != 0)
	{
		c >>= 8 - i;
		p.writeChar(c);		

		for(SyncValue *v = sync_ptr; v != sync_end; v++)
		{
			if(v->need_sync)
				v->send(p);

			if(v->dyn_sync_freq)
				v->updateOld();
		}
	}
	
}

void ValueSynchronizer::receive(Packet& p)
{
	SyncValue* sync_end = sync_ptr + sync_c;
	unsigned char c = p.readChar();
	unsigned char i = 0;
	unsigned char iq = c;
	for(int ic = 0; ic < 8; ic++)
	{
		if(c >> ic & 1)
			printf("#");
		else
			printf("_");
	}
	printf("\n");
	for (SyncValue *v = sync_ptr; v != sync_end; v++)
	{
		if(++i == 8)
		{
			c = p.readChar();
			i = 0;
		}
		if(c & 1)
		{
			v->receive(p);
		}
		c >>= 1;
	}
}

int ValueSynchronizer::getSize()
{
	int size = 0;
	SyncValue* sync_end = sync_ptr + sync_c;
	for (SyncValue *v = sync_ptr; v != sync_end; v++)
	{
		if(v->need_sync)
			size += v->getSize();
	}
	if(size == 0)
		return 0;

	return size + sizeof(char) * ((sync_c - 1) / 8 + 1);
}

void ValueSynchronizer::updateSyncList()
{
	SyncValue* sync_end = sync_ptr + sync_c;
	for (SyncValue *v = sync_ptr; v != sync_end; v++)
	{
		int freq = (v->dyn_sync_freq ? ((v->dyn_sync_decay_current > 0 || v->isDirty()) ? v->freq : v->freq_static) : v->freq);
		v->need_sync = freq && (time % freq == 0);
	}

	time++;
}