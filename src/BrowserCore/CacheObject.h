#ifndef _BrowserCore_CacheObject_h_
#define _BrowserCore_CacheObject_h_

#include "Utils.h"

class CacheObject {
	uint64 url_hash;
	uint32 size;
	
protected:
	CacheObject();
	
public:
	
	virtual ~CacheObject();
	
	void StoreThis();
	void LoadThis();
	
	uint64 GetHashValue() {return url_hash;}
	
	void SetUrl(const String& url);
	void SetSize(uint32 s) {size = s;}
	
	template <class T> bool Is() {return dynamic_cast<T*>(this) != 0;}
	template <class T> T& Get() {return dynamic_cast<T&>(*this);}
	
};

class CacheString : public CacheObject {
	String s;
public:
	CacheString();
	
	const String& GetData() const {return s;}
	void SetData(const String& s) {this->s = s;}
	
};




template <int I> inline CacheObject* LoadCacheObject(Stream& s) {
	return 0;
}
template <int I> inline CacheObject* StoreCacheObject(Stream& s) {
	return 0;
}

template <> inline CacheObject* LoadCacheObject<0>(Stream& s) {
	CacheString* cs_ = new CacheString();
	CacheString& cs = *cs_;
	
	
	return cs_;
}




class Cache {
	
protected:
	VectorMap<unsigned, CacheObject*> objs;
	Index<unsigned> existing_cache_files;
	Mutex lock;
	
	void RefreshFiles();
	
public:
	Cache();
	~Cache();
	
	int				FindCacheObject(const String& path);
	int				FindCacheObject(const String& filename, const String& path);
	
	CacheObject& 	GetCacheObject(int i);
	template <class T> T& GetAdd(const String& url) {
		unsigned hash = url.GetHashValue();
		lock.Enter();
		int i = objs.Find(hash);
		if (i == -1) {
			T* tobj = new T();
			CacheObject& obj = *objs.Add(hash, tobj);
			obj.SetUrl(url);
			lock.Leave();
			return *tobj;
		} else {
			CacheObject& old_obj = *objs[i];
			if (!old_obj.Is<T>()) {
				delete &old_obj;
				objs.Remove(i);
				lock.Leave();
				return GetAdd<T>(url);
			}
			lock.Leave();
			return old_obj.Get<T>();
		}
	};
};

inline Cache& GetCache() {return Single<Cache>();}



#endif
