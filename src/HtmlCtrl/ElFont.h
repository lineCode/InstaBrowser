#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElFont : public HtmlTag
{
public:
	ElFont(Document& doc);
	virtual ~ElFont();

	virtual void ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
