#pragma once
#include "Object.h"

NAMESPACE_HTMLCTRL

struct MediaQueryExpression : Moveable<MediaQueryExpression>
{
	typedef Vector<MediaQueryExpression>	vector;
	MediaFeature	feature;
	int				val;
	int				val2;
	bool			check_as_bool;
	
	MediaQueryExpression()
	{
		check_as_bool	= false;
		feature			= media_feature_none;
		val				= 0;
		val2			= 0;
	}

	bool Check(const MediaFeatures& features) const;
};

class MediaQuery : public Object
{
public:
	typedef ObjectPtr<MediaQuery>			ptr;
	typedef Vector<MediaQuery::ptr>	vector;
private:
	MediaQueryExpression::vector	m_expressions;
	bool							m_not;
	MediaType						m_MediaType;
public:
	MediaQuery(const MediaQuery& val);

	static MediaQuery::ptr CreateFromString(const String& str, Document* doc);
	bool Check(const MediaFeatures& features) const;
private:
	MediaQuery(ObjectPool& pool);
};

class Context;

class MediaQueryList : public Object
{
public:
	typedef ObjectPtr<MediaQueryList>		ptr;
	typedef Vector<MediaQueryList::ptr>	vector;
private:
	MediaQuery::vector	m_queries;
	bool				m_is_used;
public:
	MediaQueryList(const MediaQueryList& val);

	static MediaQueryList::ptr CreateFromString(const String& str, Document* doc);
	bool IsUsed() const;
	bool ApplyMediaFeatures(const MediaFeatures& features);	// returns true if the m_is_used changed
protected:
	friend class Context;
	MediaQueryList(ObjectPool& pool);
};

inline MediaQueryList::MediaQueryList(const MediaQueryList& val) : Object(val.GetObjectPool())
{
	m_is_used	= val.m_is_used;
	m_queries	<<= val.m_queries;
}

inline MediaQueryList::MediaQueryList(ObjectPool& pool) : Object(pool)
{
	m_is_used = false;
}

inline bool MediaQueryList::IsUsed() const
{
	return m_is_used;
}


END_HTMLCTRL_NAMESPACE
