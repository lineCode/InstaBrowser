#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElPara : public HtmlTag
{
public:
	ElPara(Document& doc);
	virtual ~ElPara();

	virtual void	ParseAttributes();

};

END_HTMLCTRL_NAMESPACE
