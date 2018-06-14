#ifndef EVENT_H
#define EVENT_H

#include "customer.h"

class event
{
public:
	bool type;
	int time;
	int count;
	customer c;
};

#endif
