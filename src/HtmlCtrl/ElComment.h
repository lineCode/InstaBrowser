#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElComment : public Element
{
	String	m_text;
public:
	ElComment(Document& doc);
	virtual ~ElComment();

	virtual void	GetText(String& text);
	virtual void	SetData(const String& data);
};

END_HTMLCTRL_NAMESPACE

