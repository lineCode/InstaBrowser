#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElTr : public HtmlTag
{
public:
	ElTr(Document& doc);
	virtual ~ElTr();

	virtual void	ParseAttributes();
	virtual void	GetInLineBoxes(Position::vector& Boxes);
};

END_HTMLCTRL_NAMESPACE
