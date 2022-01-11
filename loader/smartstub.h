#ifndef SMARTSTUB_H
#define SMARTSTUB_H

typedef struct _SmartStub 
{
	u32 moduleInfoAddr;
} SmartStub;

int smartStubStart(SmartStub *pSmartStub);
void* smartStubGetFunctionAddress(SmartStub *pSmartStub, char *libname, u32 nid);
void smartStubEnd(SmartStub *pSmartStub);

#endif
