#include "CacheObject.h"



CacheObject::CacheObject() {
	url_hash = 0;
	size = 0;
	
}

CacheObject::~CacheObject() {
	LOG(Format("~CacheObject() this=%X", (int64)this));
}

void CacheObject::SetUrl(const String& url) {
	url_hash = url.GetHashValue();
}

void CacheObject::StoreThis() {
	
}

void CacheObject::LoadThis() {
	
}





CacheString::CacheString() {
	
}






Cache::Cache() {
	
}

Cache::~Cache() {
	lock.Enter();
	for(int i = 0; i < objs.GetCount(); i++) {
		delete objs[i];
	}
	objs.Clear();
	lock.Leave();
}

void Cache:: RefreshFiles() {
	
}

int	Cache::FindCacheObject(const String& path) {
	LOG("FindCacheObject " << path);
	lock.Enter();
	int ret = objs.Find(path.GetHashValue());
	//LOG("FindCacheObject \"" << path << "\" id=" << ret << " hashvalue=" << IntStr(path.GetHashValue()) );
	lock.Leave();
	return ret;
}

int	Cache::FindCacheObject(const String& path, const String& baseurl) {
	if (PathIsURL(path))
		return FindCacheObject(path);
	String s;
	MakeUrl(path, baseurl, s);
	return FindCacheObject(s);
}


CacheObject& Cache::GetCacheObject(int i) {
	lock.Enter();
	CacheObject& co = *objs[i];
	lock.Leave();
	return co;
}
