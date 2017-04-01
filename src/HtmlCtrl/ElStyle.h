#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElStyle : public Element
{
	ElementsVector		m_children;
public:
	ElStyle(Document& doc);
	virtual ~ElStyle();

	virtual void			ParseAttributes();
	virtual bool			AppendChild(Element* el);
	virtual String			GetTagName() const;
};

END_HTMLCTRL_NAMESPACE
