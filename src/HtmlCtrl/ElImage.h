#pragma once

#include "HtmlTag.h"

NAMESPACE_HTMLCTRL


class ElImage : public HtmlTag
{
	String	m_src;
	int		cache_id;
	
public:
	ElImage(Document& doc);
	virtual ~ElImage(void);

	virtual int		LineHeight() const;
	virtual bool	IsReplaced() const;
	virtual int		Render(int x, int y, int max_width, bool second_pass = false);
	virtual void	ParseAttributes();
	virtual void	ParseStyles(bool is_reparse = false);
	virtual void	Paint(Draw& hdc, int x, int y, const Position& clip);
	virtual void	GetContentSize(Size& sz, int max_width);
};

END_HTMLCTRL_NAMESPACE
