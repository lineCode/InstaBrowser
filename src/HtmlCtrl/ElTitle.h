#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElTitle : public HtmlTag
{
public:
	ElTitle(Document& doc);
	virtual ~ElTitle();

protected:
	virtual void	ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
