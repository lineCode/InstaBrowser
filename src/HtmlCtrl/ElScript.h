#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElScript : public Element
{
	String m_text;
	String type;
public:
	ElScript(Document& doc);
	virtual ~ElScript();

	virtual void			ParseAttributes();
	virtual bool			AppendChild(Element* el);
	virtual String			GetTagName() const;
	virtual void			SetAttr(const String& name, const String& val);
	
	const String& GetContent() const {return m_text;}
	
};

END_HTMLCTRL_NAMESPACE

