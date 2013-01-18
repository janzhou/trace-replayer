#ifndef KS706_H
#define KS706_H

#include "DMPrivate.h"

Connection * KS706_Connect(Device_Node* device);
int KS706_DeviceStart(Connection * current_connection);
int KS706_DeviceStop(Connection * current_connection);
int KS706_Disconnect(Connection * current_connection);
static void * KS706_DoSample(void * point);

#endif

