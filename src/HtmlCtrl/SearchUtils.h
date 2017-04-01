#ifndef _HtmlCtrl_SearchUtils_h_
#define _HtmlCtrl_SearchUtils_h_


#include <Core/Core.h>
using namespace Upp;
using namespace Upp;

// equal to String::Find(), but finds the first position of any character specified
int FindFirstOf(const String& src, const String& match, int startpos=0);
int FindFirstOf(const String& src, char match, int startpos=0);

// equal to FindFirstOf(), but finds the last position of any character specified 
int FindLastOf(const String& src, const String& match, int endpos=0);

// equal to FindFirstOf(), but returns the position of the first character not of those specified
int FindFirstNotOf(const String& src, const String& match, int startpos=0);

// equal to FindLastOf(), but returns the last position of any characters not specified
int FindLastNotOf(const String& src, const String& match, int endpos=0);


template <class T, class V>
int FindPos(const Vector<T>& vec, const V& value)
{
	for(int i = 0; i < vec.GetCount(); i++) {
		if (vec[i] == value)
			return i;
	}
	return -1;
}

template <class T, class V>
V& GetSet(ArrayMap<T, V>& amap, const T& key) {
	int i = amap.Find(key);
	if (i == -1)
		return amap.Add(key);
	return amap[i];
}
#endif
