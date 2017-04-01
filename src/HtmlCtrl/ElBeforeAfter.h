#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElBeforeAfterBase : public HtmlTag
{
public:
	ElBeforeAfterBase(Document& doc, bool before);
	virtual ~ElBeforeAfterBase();

	virtual void AddStyle(Style::ptr st);
	virtual void ApplyStylesheet(const Css& Stylesheet);
private:
	void	AddText(const String& txt);
	void	AddFunction(const String& fnc, const String& params);
	wchar ConvertEscape(const String& txt);
};

class ElBefore : public ElBeforeAfterBase
{
public:
	ElBefore(Document& doc) : ElBeforeAfterBase(doc, true)
	{

	}
};

class ElAfter : public ElBeforeAfterBase
{
public:
	ElAfter(Document& doc) : ElBeforeAfterBase(doc, false)
	{

	}
};

END_HTMLCTRL_NAMESPACE
