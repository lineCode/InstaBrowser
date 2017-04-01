#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElTd : public HtmlTag
{
public:
	ElTd(Document& doc);
	virtual ~ElTd();

	virtual void ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
