#pragma once

#include "Common.h"
#include "MemoryDbg.h"

NAMESPACE_HTMLCTRL

class Object
{
#if DEBUG_MEMORY
	ObjectPool* pool;
#endif

protected:
	int	m_refCount;
public:
	Object(ObjectPool& pool)
	{
#if DEBUG_MEMORY
		this->pool = &pool;
		pool.MarkAllocated(this);
#endif		
		m_refCount = 0;
	}
	virtual ~Object()
	{
#if DEBUG_MEMORY
		pool->MarkDeallocated(this);
#endif
	}

#if DEBUG_MEMORY
	ObjectPool& GetObjectPool() const {return *pool;}
#endif
	int GetRefs() {return m_refCount;}
	
	void Ref()
	{
		m_refCount++;
	}
	
	void Unref()
	{
		ASSERT(m_refCount > 0);
		if(!(--m_refCount)) delete this;
	}
};

template<class T>
class ObjectPtr : public Moveable<ObjectPtr<T> >
{
	T*	m_ptr;
public:
	ObjectPtr()
	{
		m_ptr = 0;
	}

	ObjectPtr(T* ptr)
	{
		m_ptr = ptr;
		if(m_ptr)
		{
			m_ptr->Ref();
		}
	}

	ObjectPtr(const ObjectPtr<T>& val)
	{
		m_ptr = val.m_ptr;
		if(m_ptr)
		{
			m_ptr->Ref();
		}
	}

	~ObjectPtr()
	{
		Clear();
	}
	
	void Clear() {
		if(m_ptr)
		{
			m_ptr->Unref();
		}
		m_ptr = 0;
	}

	void operator=(const ObjectPtr<T>& val)
	{
		T* oldPtr = m_ptr;
		m_ptr = val.m_ptr;
		if(m_ptr)
		{
			m_ptr->Ref();
		}
		if(oldPtr)
		{
			oldPtr->Unref();
		}
	}

	void operator=(T* val)
	{
		T* oldPtr = m_ptr;
		m_ptr = val;
		if(m_ptr)
		{
			m_ptr->Ref();
		}
		if(oldPtr)
		{
			oldPtr->Unref();
		}
	}

	T* operator->()
	{
		return m_ptr;
	}

	const T* operator->() const
	{
		return m_ptr;
	}

	operator const T*() const {return m_ptr;}
	operator T*() {return m_ptr;}

};

END_HTMLCTRL_NAMESPACE

