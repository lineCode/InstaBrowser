#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElDiv : public HtmlTag
{
public:
	ElDiv(Document& doc);
	virtual ~ElDiv();

	virtual void ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
