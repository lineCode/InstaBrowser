#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElAnchor : public HtmlTag
{
public:
	ElAnchor(Document& doc);
	virtual ~ElAnchor();

	virtual void	OnClick();
	virtual void	ApplyStylesheet(const Css& Stylesheet);
};

END_HTMLCTRL_NAMESPACE
