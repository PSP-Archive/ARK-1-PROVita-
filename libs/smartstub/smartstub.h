#ifndef SMARTSTUB_H
#define SMARTSTUB_H

typedef struct _SmartStub 
{
	u32 moduleInfoAddr;
	u32 baseAddr;
	u32 startAddr;
	u32 endAddr;
} SmartStub;

int smartStubStart(SmartStub *pSmartStub);
int smartStubSearch(SmartStub *pSmartStub);
void* smartStubGetFunctionAddress(SmartStub *pSmartStub, char *libname, u32 nid);
void *smartStubSearchWholeMemory(char *libname, u32 nid);
int smartStubSearchLibrary(SmartStub *stub, char *libname);

#endif
