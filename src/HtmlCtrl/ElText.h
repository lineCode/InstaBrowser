#pragma once

#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

class ElText : public Element
{
protected:
	String			m_text;
	String			m_transformed_text;
	Size			m_size;
	TextTransform	m_text_transform;
	bool			m_use_transformed;
	bool			m_draw_spaces;
public:
	ElText(const String& text, Document& doc);
	virtual ~ElText();

	virtual void				GetText(String& text);
	virtual String				GetStyleProperty(const String& name, bool inherited, const String& def = "");
	virtual void				ParseStyles(bool is_reparse);
	virtual int					GetBaseLine();
	virtual void				Paint(Draw& hdc, int x, int y, const Position& clip);
	virtual int					LineHeight() const;
	virtual uint32				GetFont(FontMetrics* fm = 0);
	virtual StyleDisplay		GetDisplay() const;
	virtual WhiteSpace			GetWhiteSpace() const;
	virtual ElementPosition		GetElementPosition(CssOffsets* offsets = 0) const;
	virtual CssOffsets			GetCssOffsets() const;

protected:
	virtual void				GetContentSize(Size& sz, int max_width);
};

END_HTMLCTRL_NAMESPACE
