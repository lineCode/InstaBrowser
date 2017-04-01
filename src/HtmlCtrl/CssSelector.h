#pragma once
#include "Style.h"
#include "MediaQuery.h"

NAMESPACE_HTMLCTRL

struct SelectorSpecificity
{
	int		a;
	int		b;
	int		c;
	int		d;

	SelectorSpecificity(int va = 0, int vb = 0, int vc = 0, int vd = 0)
	{
		a	= va;
		b	= vb;
		c	= vc;
		d	= vd;
	}

	void operator += (const SelectorSpecificity& val)
	{
		a	+= val.a;
		b	+= val.b;
		c	+= val.c;
		d	+= val.d;
	}

	bool operator==(const SelectorSpecificity& val) const
	{
		if(a == val.a && b == val.b && c == val.c && d == val.d)
		{
			return true;
		}
		return false;
	}

	bool operator!=(const SelectorSpecificity& val) const
	{
		if(a != val.a || b != val.b || c != val.c || d != val.d)
		{
			return true;
		}
		return false;
	}

	bool operator > (const SelectorSpecificity& val) const
	{
		if(a > val.a)
		{
			return true;
		} else if(a < val.a)
		{
			return false;
		} else
		{
			if(b > val.b)
			{
				return true;
			} else if(b < val.b)
			{
				return false;
			} else
			{
				if(c > val.c)
				{
					return true;
				} else if(c < val.c)
				{
					return false;
				} else
				{
					if(d > val.d)
					{
						return true;
					} else if(d < val.d)
					{
						return false;
					}
				}
			}
		}
		return false;
	}

	bool operator >= (const SelectorSpecificity& val) const
	{
		if((*this) == val) return true;
		if((*this) > val) return true;
		return false;
	}

	bool operator <= (const SelectorSpecificity& val) const
	{
		if((*this) > val)
		{
			return false;
		}
		return true;
	}

	bool operator < (const SelectorSpecificity& val) const
	{
		if((*this) <= val && (*this) != val)
		{
			return true;
		}
		return false;
	}

};

enum AttrSelectCondition
{
	select_exists,
	select_equal,
	select_contain_str,
	select_start_str,
	select_end_str,
	select_pseudo_class,
	select_pseudo_element,
};

struct CssAttributeSelector : Moveable<CssAttributeSelector>
{
	typedef Vector<CssAttributeSelector>	vector;

	String					attribute;
	String					val;
	AttrSelectCondition	condition;

	CssAttributeSelector()
	{
		condition = select_exists;
	}

	CssAttributeSelector(const CssAttributeSelector& val)
	{
		this->val	= val.val;
		condition	= val.condition;
		attribute	= val.attribute;
	}
};


class CssElementSelector
{
public:
	String							m_tag;
	CssAttributeSelector::vector	m_attrs;
public:
	CssElementSelector()
	{

	}

	CssElementSelector(const CssElementSelector& val);

	void Parse(const String& txt);
	CssElementSelector& operator = (const CssElementSelector& es);
};


enum CssCombinator
{
	combinator_descendant,
	combinator_child,
	combinator_adjacent_sibling,
	combinator_general_sibling
};


class CssSelector : public Object, public Moveable<CssSelector>
{
public:
	typedef ObjectPtr<CssSelector>		ptr;
	typedef Vector<CssSelector::ptr>	vector;
public:
	SelectorSpecificity		m_specificity;
	CssElementSelector		m_right;
	CssSelector::ptr		m_left;
	CssCombinator			m_combinator;
	Style::ptr				m_style;
	int						m_order;
	MediaQueryList::ptr	m_media_query;
public:
	CssSelector(ObjectPool& pool, MediaQueryList::ptr media) : Object(pool)
	{
		m_media_query	= media;
		m_combinator	= combinator_descendant;
		m_order			= 0;
	}

	~CssSelector()
	{
	}

	CssSelector(const CssSelector& val) : Object(val.GetObjectPool())
	{
		m_right			= val.m_right;
		if(val.m_left)
		{
			m_left			= new CssSelector(*val.m_left);
		} else
		{
			m_left = 0;
		}
		m_combinator	= val.m_combinator;
		m_specificity	= val.m_specificity;
		m_order			= val.m_order;
		m_media_query	= val.m_media_query;
	}

	bool Parse(const String& text);
	void CalcSpecificity();
	bool IsMediaValid() const;
	void AddMediaToDoc(Document* doc) const;
};

inline bool CssSelector::IsMediaValid() const
{
	if(!m_media_query)
	{
		return true;
	}
	return m_media_query->IsUsed();
}


inline bool operator > (const CssSelector& v1, const CssSelector& v2)
{
	if(v1.m_specificity == v2.m_specificity)
	{
		return (v1.m_order > v2.m_order);
	}
	return (v1.m_specificity > v2.m_specificity);
}

inline bool operator < (const CssSelector& v1, const CssSelector& v2)
{
	if(v1.m_specificity == v2.m_specificity)
	{
		return (v1.m_order < v2.m_order);
	}
	return (v1.m_specificity < v2.m_specificity);
}

inline bool operator > (CssSelector::ptr v1, CssSelector::ptr v2)
{
	return (*v1 > *v2);
}

inline bool operator < (CssSelector::ptr v1, CssSelector::ptr v2)
{
	return (*v1 < *v2);
}





class UsedSelector : public Object
{
public:
	typedef ObjectPtr<UsedSelector>		ptr;
	typedef Vector<UsedSelector::ptr>	vector;

	CssSelector::ptr	m_selector;
	bool				m_used;

	UsedSelector(CssSelector::ptr selector, bool used) : Object(selector->GetObjectPool())
	{
		m_used		= used;
		m_selector	= selector;
	}
};

END_HTMLCTRL_NAMESPACE
