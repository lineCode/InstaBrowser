#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElCdata : public Element
{
	String	m_text;
public:
	ElCdata(Document& doc);
	virtual ~ElCdata();

	virtual void	GetText(String& text);
	virtual void	SetData(const String& data);
};

END_HTMLCTRL_NAMESPACE
