#include "Html.h"
#include "ElText.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElText::ElText( const String& text, Document& doc ) : Element(doc)
{
	if(text.GetCount())
	{
		m_text = text;
	}
	m_text_transform	= TextTransform_none;
	m_use_transformed	= false;
	m_draw_spaces		= true;
}

ElText::~ElText()
{

}

void ElText::GetContentSize( Size& sz, int max_width )
{
	sz = m_size;
}

void ElText::GetText( String& text )
{
	text += m_text;
}

String ElText::GetStyleProperty( const String& name, bool inherited, const String& def /*= 0*/ )
{
	if(inherited)
	{
		Element* e = GetParentElement();
		if (!e) return def;
		return e->GetStyleProperty(name, inherited, def);
	}
	return def;
}

void ElText::ParseStyles(bool is_reparse)
{
	m_text_transform	= (TextTransform)	ValueIndex(GetStyleProperty("text-transform", true,	"none"),	TextTransform_strings,	TextTransform_none);
	if(m_text_transform != TextTransform_none)
	{
		m_transformed_text	= m_text;
		m_use_transformed = true;
		GetDocument()->GetHtmlFile()->TransformText(m_transformed_text, m_text_transform);
	}

	if(IsWhiteSpace())
	{
		m_transformed_text = " ";
		m_use_transformed = true;
	} else
	{
		if(m_text == "\t")
		{
			m_transformed_text = "    ";
			m_use_transformed = true;
		}
		if(m_text == "\n" || m_text == "\r")
		{
			m_transformed_text = "";
			m_use_transformed = true;
		}
	}
	
	
	FontMetrics fm;
	Element* e = GetParentElement();
	
	uint32 font	= e->GetFont(&fm);
	if (font >= 1000) {
		font	= e->GetFont(&fm);
		Panic("errorz here");
	}


	if(IsBreak())
	{
		m_size.cy	= 0;
		m_size.cx	= 0;
	} else
	{
		m_size.cy	= fm.height;
		m_size.cx	= GetDocument()->GetHtmlFile()->TextWidth(m_use_transformed ? m_transformed_text : m_text, font);
	}
	
	m_draw_spaces = fm.draw_spaces;

	
}

int ElText::GetBaseLine()
{
	return GetParentElement()->GetBaseLine();
}

void ElText::Paint( Draw& hdc, int x, int y, const Position& clip )
{
	if(IsWhiteSpace() && !m_draw_spaces)
	{
		return;
	}

	Position pos = m_pos;
	pos.x	+= x;
	pos.y	+= y;

	if(pos.DoesIntersect(clip))
	{
		uint32 font = GetParentElement()->GetFont();
		WebColor color = GetParentElement()->GetColor("color", true, GetDocument()->GetDefColor());
		GetDocument()->GetHtmlFile()->DrawText(hdc, m_use_transformed ? m_transformed_text : m_text, font, color, pos);
	}
}

int ElText::LineHeight() const
{
	return GetParentElement()->LineHeight();
}

uint32 ElText::GetFont( FontMetrics* fm /*= 0*/ )
{
	return GetParentElement()->GetFont(fm);
}

StyleDisplay ElText::GetDisplay() const
{
	return display_inline_text;
}

WhiteSpace ElText::GetWhiteSpace() const
{
	Element* m_higher = GetParentElement();
	if(m_higher) return m_higher->GetWhiteSpace();
	return white_space_normal;
}

ElementPosition ElText::GetElementPosition(CssOffsets* offsets) const
{
	Element* p = GetParentElement();
	while(p && p->GetDisplay() == display_inline)
	{
		if(p->GetElementPosition() == element_position_relative)
		{
			if(offsets)
			{
				*offsets = p->GetCssOffsets();
			}
			return element_position_relative;
		}
		p = p->GetParentElement();
	}
	return element_position_static;
}

CssOffsets ElText::GetCssOffsets() const
{
	Element* p = GetParentElement();
	while(p && p->GetDisplay() == display_inline)
	{
		if(p->GetElementPosition() == element_position_relative)
		{
			return p->GetCssOffsets();
		}
		p = p->GetParentElement();
	}
	return CssOffsets();
}

END_HTMLCTRL_NAMESPACE
