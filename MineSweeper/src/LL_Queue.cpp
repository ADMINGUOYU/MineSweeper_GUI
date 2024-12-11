#include "LL_Queue.h"
#include <iostream>
#include <limits>

LL_Queue::LL_Queue(void)
	:
	head(nullptr),
	count(0)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'LL_Queue' constructor being called\n";
#endif // debug message (you will see this message printed when Visual Studio is in DEBUG config)
	return;
}

LL_Queue::~LL_Queue(void)
{
#ifdef _DEBUG
	std::wcout << this << " CLASS: 'LL_Queue' destructor being called\n";
#endif
	this->clearQueue();

#ifdef _DEBUG
	std::wcout << this << " CLASS: 'LL_Queue' destructor execution completes\n";
#endif
	return;
}

bool LL_Queue::enqueue(unsigned int data, unsigned int type)
{
#ifdef _DEBUG
	std::wcout << this << " \t'LL_Queue 'enqueue''"
		<< " data: " << data 
		<< " type: " << type
		<< "\n";
#endif
	if (nullptr == this->head)
	{
		this->head = new LL_NodeData();
		if (nullptr == this->head) return false;
		this->head->data = data;
		this->head->type = type;
		this->head->next = nullptr;
	}
	else 
	{
		LL_NodeData* ptrTemp = this->head;
		for ( ; nullptr != ptrTemp->next; ptrTemp = ptrTemp->next) { }

		ptrTemp->next = new LL_NodeData();
		ptrTemp = ptrTemp->next;
		if (nullptr == ptrTemp) return false;
		ptrTemp->data = data;
		ptrTemp->type = type;
		ptrTemp->next = nullptr;
	}

	this->count++;
#ifdef _DEBUG
	std::wcout << this << " \t'LL_Queue 'enqueue'' success count: " << this->count << "\n";
#endif
	return true;
}

unsigned int LL_Queue::dequeue(unsigned int* ptrData)
{
	// nothing to dequeue
	if (!this->count) return UINT_MAX;

	// now dequeue
	unsigned int data = this->head->data;
	unsigned int type = this->head->type;
	LL_NodeData* ptrTemp = this->head->next;
	delete this->head;
	this->head = ptrTemp;
	this->count--;
	*ptrData = data;

#ifdef _DEBUG
	std::wcout << this << " \t'LL_Queue 'dequeue''"
		<< " data: " << data 
		<< " type: " << type
		<< " count: " << this->count 
		<< "\n";
#endif
	
	return type;
}

bool LL_Queue::clearQueue(void)
{
	if (nullptr == this->head)
		return false;

	for (LL_NodeData* ptrTemp = nullptr; nullptr != this->head; )
	{
		ptrTemp = this->head->next;
		delete this->head;
		this->head = ptrTemp;
	}

	this->head = nullptr;
	this->count = 0;

	return true;
}


