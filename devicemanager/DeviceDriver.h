#ifndef DEVICEDRIVER_H
#define DEVICEDRIVER_H
#include "DeviceManager.h"

typedef struct Connection{
	Device dev;
	int drive_id;
	int sock_id;
	void * data;
	struct Connection* next;
} Connection;

typedef struct Drive{
	char type[20];
	int (* DevConnect)(Connection *);
	int (* DevStart)(Connection *);
	int (* DevStop)(Connection *);
	int (* DevDisconnect)(Connection *);
	int (* GetPortInfo)(Connection *, PortInfo PInfo[],int);
	int (* SetPort)(Connection *, int port_id, float * pValue);
	int (* IsPortUsed)(Connection *, int port_id);
	int (* DeletePort)(Connection *, int port_id);
}Drive;

extern int DriveReg(struct Drive *);

#endif
