#pragma once
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

struct col_info
{
	int		width;
	bool	is_auto;
};


class ElTable : public HtmlTag
{
public:
	ElTable(Document& doc);
	virtual ~ElTable();

	virtual bool	AppendChild(Element* el);
	virtual void	ParseStyles(bool is_reparse = false);
	virtual void	ParseAttributes();
};

END_HTMLCTRL_NAMESPACE
