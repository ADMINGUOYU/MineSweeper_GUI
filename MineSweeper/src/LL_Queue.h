#pragma once

#ifndef _LL_QUEUE_
#define _LL_QUEUE_

#define ENCODE_DWORD(l,h) (l | (h << 16))
#define GET_LOWORD(l) ((WORD)(l) & 0xffff)
#define GET_HIWORD(h) ((WORD)(h >> 16) & 0xffff)

class LL_Queue
{
private:
	struct LL_NodeData
	{
		unsigned int data;
		unsigned int type;
		LL_NodeData* next;
	};

private:
	LL_NodeData* head;
	unsigned int count;

public:
	bool enqueue(unsigned int data, unsigned int type);
	unsigned int dequeue(unsigned int* ptrData);
	bool clearQueue(void);

public:
	bool isEmpty(void) const { return (this->count == 0) ? true : false; }
	unsigned int getCount(void) const { return this->count; }

public:
	LL_Queue(void);
	~LL_Queue(void);

};

#endif // !_LL_QUEUE_


