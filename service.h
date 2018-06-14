#ifndef SERVICE_H
#define SERVICE_H

#include"customer.h"

class service
{
public:
	vector<bool> busy_or_not;
	queue<customer> service_customers;
};

#endif
