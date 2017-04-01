#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElBase : public HtmlTag
{
public:
	ElBase(Document& doc);
	virtual ~ElBase();

	virtual void	ParseAttributes();
};

END_HTMLCTRL_NAMESPACE

