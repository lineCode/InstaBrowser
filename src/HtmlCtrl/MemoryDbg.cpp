#include "Object.h"

NAMESPACE_HTMLCTRL


ObjectPool::ObjectPool() {
	
}

ObjectPool::~ObjectPool() {
	//ShowAllocatedObjects();
	ForceCleanAllocatedObjects();
}

void ObjectPool::MarkAllocated(Object *obj) {
	allocatedObj.Add(obj);
}

void ObjectPool::MarkDeallocated(Object *obj) {
	for (int i = allocatedObj.GetCount()-1; i >= 0; i--) {
		if (allocatedObj[i] == obj) {
			allocatedObj.Remove(i);
		}
	}
}

void ObjectPool::ShowAllocatedObjects() {
	LOG("Allocated objects:");
	for(int i = 0; i < allocatedObj.GetCount(); i++) {
		Object* obj = allocatedObj[i];
		LOG(Format(" %d: addr=%X refs=%d", i, (int64)obj, obj->GetRefs()));
	}
}

void ObjectPool::ForceCleanAllocatedObjects() {
	while (allocatedObj.GetCount()) {
		int pos = allocatedObj.GetCount() - 1;
		Object* obj = allocatedObj[pos];
		allocatedObj.Remove(pos);
		delete obj;
	}
}

int ObjectPool::GetMemoryObjectCount() {
	return allocatedObj.GetCount();
}

Object* ObjectPool::GetMemoryVar(int i) {
	return allocatedObj[i];
}

END_HTMLCTRL_NAMESPACE
