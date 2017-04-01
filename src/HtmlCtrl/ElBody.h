#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElBody : public HtmlTag
{
public:
	ElBody(Document& doc);
	virtual ~ElBody();

	virtual bool IsBody()  const;
	
	virtual void ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
