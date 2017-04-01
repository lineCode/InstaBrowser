#pragma once
#include "HtmlTag.h"
#include "ElText.h"

NAMESPACE_HTMLCTRL

class ElSpace : public ElText
{
public:
	ElSpace(const String& text, Document& doc);
	virtual ~ElSpace();

	virtual bool	IsWhiteSpace();
	virtual bool	IsBreak() const;
};

END_HTMLCTRL_NAMESPACE
