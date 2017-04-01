#ifndef _HtmlCtrl_MemoryDbg_h_
#define _HtmlCtrl_MemoryDbg_h_

#include "Common.h"

#define DEBUG_MEMORY 1

NAMESPACE_HTMLCTRL
class Object;

class ObjectPool {
	Vector<Object*> allocatedObj;
	
public:
	ObjectPool();
	~ObjectPool();
	
	void MarkAllocated(Object *obj);
	void MarkDeallocated(Object *obj);
	void ShowAllocatedObjects();
	void ForceCleanAllocatedObjects();
	
	int GetMemoryObjectCount();
	Object* GetMemoryVar(int i);
};
END_HTMLCTRL_NAMESPACE

#endif
