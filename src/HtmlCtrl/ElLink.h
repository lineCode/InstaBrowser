#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElLink : public HtmlTag
{
public:
	ElLink(Document& doc);
	virtual ~ElLink();

protected:
	virtual void	ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
