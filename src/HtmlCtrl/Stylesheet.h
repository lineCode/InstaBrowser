#pragma once
#include "Style.h"
#include "CssSelector.h"

class FileDirectory;

NAMESPACE_HTMLCTRL



class Css : public Object
{
	CssSelector::vector	m_selectors;
public:
	Css(ObjectPool& pool) : Object(pool)
	{

	}
	
	Css(const Css& src) : Object(src.GetObjectPool()) {
		m_selectors <<= src.m_selectors;
	}
	
	~Css()
	{

	}

	const CssSelector::vector& Selectors() const
	{
		return m_selectors;
	}

	void Clear()
	{
		m_selectors.Clear();
	}

	void	ParseStylesheet(const String& str, const String& baseurl, Document* doc, MediaQueryList::ptr& media);
	void	SortSelectors();
	static void	ParseCssUrl(const String& str, String& url);
	
	Css& operator = (const Css& css) {
		m_selectors <<= css.m_selectors; return *this;}
private:
	void	ParseAtRule(const String& text, const String& baseurl, Document* doc, MediaQueryList::ptr& media);
	void	AddSelector(CssSelector::ptr Selector);
	bool	ParseSelectors(const String& txt, Style::ptr Styles, MediaQueryList::ptr& media);

};

inline void Css::AddSelector( CssSelector::ptr Selector )
{
	Selector->m_order = (int) m_selectors.GetCount();
	m_selectors.Add(Selector);
}

END_HTMLCTRL_NAMESPACE
