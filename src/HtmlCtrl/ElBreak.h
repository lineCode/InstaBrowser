#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElBreak : public HtmlTag
{
public:
	ElBreak(Document& doc);
	virtual ~ElBreak();

	virtual bool	IsBreak() const;
};

END_HTMLCTRL_NAMESPACE
