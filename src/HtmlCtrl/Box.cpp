#include "Html.h"
#include "Box.h"
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

Box_type BlockBox::GetType()
{
	return Box_block;
}

int BlockBox::Height()
{
	return m_element->Height();
}

int BlockBox::Width()
{
	return m_element->Width();
}

void BlockBox::AddElement( Element* el)
{
	m_element = el;
	el->m_box = this;
}

void BlockBox::Finish(bool last_box)
{
	if(!m_element) return;

	CssOffsets offsets;
	if(m_element->GetElementPosition(&offsets) == element_position_relative)
	{
		if(!offsets.left.IsPredefined())
		{
			m_element->m_pos.x += offsets.left.CalcPercent(m_box_right - m_box_left);
		} else if(!offsets.right.IsPredefined())
		{
			m_element->m_pos.x -= offsets.right.CalcPercent(m_box_right - m_box_left);
		}
		if(!offsets.top.IsPredefined())
		{
			int h = 0;
			if(offsets.top.Units() == css_units_percentage)
			{
				if(m_element->GetParentElement())
				{
					m_element->GetParentElement()->GetPredefinedHeight(h);
				}
			}
			m_element->m_pos.y += offsets.top.CalcPercent(h);
		} else if(!offsets.bottom.IsPredefined())
		{
			int h = 0;
			if(offsets.bottom.Units() == css_units_percentage)
			{
				if(m_element->GetParentElement())
				{
					m_element->GetParentElement()->GetPredefinedHeight(h);
				}
			}
			m_element->m_pos.y -= offsets.bottom.CalcPercent(h);
		}
	}
}

bool BlockBox::CanHold( Element* el, WhiteSpace ws )
{
	if(m_element || el->IsInLineBox())
	{
		return false;
	}
	return true;
}

bool BlockBox::IsEmpty()
{
	if(m_element)
	{
		return false;
	}
	return true;
}

int BlockBox::Baseline()
{
	if(m_element)
	{
		return m_element->GetBaseLine();
	}
	return 0;
}

void BlockBox::GetElements( ElementsVector& els )
{
	els.Add(m_element);
}

int BlockBox::TopMargin()
{
	if(m_element && m_element->CollapseTopMargin())
	{
		return m_element->m_margins.top;
	}
	return 0;
}

int BlockBox::BottomMargin()
{
	if(m_element && m_element->CollapseBottomMargin())
	{
		return m_element->m_margins.bottom;
	}
	return 0;
}

void BlockBox::SetYShift( int shift )
{
	m_box_top += shift;
	if(m_element)
	{
		m_element->m_pos.y += shift;
	}
}

void BlockBox::NewWidth( int left, int right, ElementsVector& els )
{

}










Box_type LineBox::GetType()
{
	return Box_line;
}

int LineBox::Height()
{
	return m_height;
}

int LineBox::Width()
{
	return m_width;
}

void LineBox::AddElement( Element* el )
{
	el->m_skip	= false;
	el->m_box	= 0;
	bool add	= true;
	if( (m_items.IsEmpty() && el->IsWhiteSpace()) || el->IsBreak() ) {
		el->m_skip = true;
	} else if(el->IsWhiteSpace()) {
		Element* ws = GetLastSpace();
		if(ws) {
			add = false;
			el->m_skip = true;
		}
	}

	if(add)
	{
		el->m_box = this;
		m_items.Add(el);

		if(!el->m_skip)
		{
			int el_shift_left	= el->GetInlineShiftLeft();
			int el_shift_right	= el->GetInlineShiftRight();

			el->m_pos.x	= m_box_left + m_width + el_shift_left + el->ContentMarginsLeft();
			el->m_pos.y	= m_box_top + el->ContentMarginsTop();
			m_width		+= el->Width() + el_shift_left + el_shift_right;
		}
	}
}

void LineBox::Finish(bool last_box)
{
	if( IsEmpty() || (!IsEmpty() && last_box && IsBreakOnly()) )
	{
		m_height = 0;
		return;
	}

	for(int j = m_items.GetCount() - 1; j >= 0; j--) {
		Element& e = *m_items[j];
		if(e.IsWhiteSpace() || e.IsBreak())
		{
			if(!e.m_skip)
			{
				e.m_skip = true;
				m_width -= e.Width();
			}
		} else
		{
			break;
		}
	}

	int baseline	= m_font_metrics.BaseLine();
	int line_height = m_line_height;

	int add_x = 0;
	switch(m_text_align)
	{
	case text_align_right:
		if(m_width < (m_box_right - m_box_left))
		{
			add_x = (m_box_right - m_box_left) - m_width;
		}
		break;
	case text_align_center:
		if(m_width < (m_box_right - m_box_left))
		{
			add_x = ((m_box_right - m_box_left) - m_width) / 2;
		}
		break;
	default:
		add_x = 0;
	}

	m_height = 0;
	
	// find line Box baseline and line-height
	for(size_t i = 0; i < m_items.GetCount(); i++)
	{
		if(m_items[i]->GetDisplay() == display_inline_text)
		{
			FontMetrics fm;
			m_items[i]->GetFont(&fm);
			baseline	= max(baseline,	fm.BaseLine());
			line_height	= max(line_height,	m_items[i]->LineHeight());
			m_height = max(m_height, fm.height);
		}
		m_items[i]->m_pos.x += add_x;
	}

	if(m_height)
	{
		baseline += (line_height - m_height) / 2;
	}

	m_height = line_height;

	int y1	= 0;
	int y2	= m_height;

	for(size_t i = 0; i < m_items.GetCount(); i++)
	{
		if(m_items[i]->GetDisplay() == display_inline_text)
		{
			FontMetrics fm;
			m_items[i]->GetFont(&fm);
			m_items[i]->m_pos.y = m_height - baseline - fm.ascent;
		} else
		{
			switch(m_items[i]->GetVerticalAlign())
			{
			case va_super:
			case va_sub:
			case va_baseline:
				m_items[i]->m_pos.y = m_height - baseline - m_items[i]->Height() + m_items[i]->GetBaseLine() + m_items[i]->ContentMarginsTop();
				break;
			case va_top:
				m_items[i]->m_pos.y = y1 + m_items[i]->ContentMarginsTop();
				break;
			case va_text_top:
				m_items[i]->m_pos.y = m_height - baseline - m_font_metrics.ascent + m_items[i]->ContentMarginsTop();
				break;
			case va_middle:
				m_items[i]->m_pos.y = m_height - baseline - m_font_metrics.x_height / 2 - m_items[i]->Height() / 2 + m_items[i]->ContentMarginsTop();
				break;
			case va_bottom:
				m_items[i]->m_pos.y = y2 - m_items[i]->Height() + m_items[i]->ContentMarginsTop();
				break;
			case va_text_bottom:
				m_items[i]->m_pos.y = m_height - baseline + m_font_metrics.descent - m_items[i]->Height() + m_items[i]->ContentMarginsTop();
				break;
			}
			y1 = min(y1, m_items[i]->Top());
			y2 = max(y2, m_items[i]->Bottom());
		}
	}

	CssOffsets offsets;

	for(size_t i = 0; i < m_items.GetCount(); i++)
	{
		m_items[i]->m_pos.y -= y1;
		m_items[i]->m_pos.y += m_box_top;
		if(m_items[i]->GetDisplay() != display_inline_text)
		{
			switch(m_items[i]->GetVerticalAlign())
			{
			case va_top:
				m_items[i]->m_pos.y = m_box_top + m_items[i]->ContentMarginsTop();
				break;
			case va_bottom:
				m_items[i]->m_pos.y = m_box_top + (y2 - y1) - m_items[i]->Height() + m_items[i]->ContentMarginsTop();
				break;
			case va_baseline:
				//TODO: process vertical align "baseline"
				break;
			case va_middle:
				//TODO: process vertical align "middle"
				break;
			case va_sub:
				//TODO: process vertical align "sub"
				break;
			case va_super:
				//TODO: process vertical align "super"
				break;
			case va_text_bottom:
				//TODO: process vertical align "text-bottom"
				break;
			case va_text_top:
				//TODO: process vertical align "text-top"
				break;
			}
		}

		// update Position for relative Positioned elements
		if(m_items[i]->GetElementPosition(&offsets) == element_position_relative)
		{
			if(!offsets.left.IsPredefined())
			{
				m_items[i]->m_pos.x += offsets.left.CalcPercent(m_box_right - m_box_left);
			} else if(!offsets.right.IsPredefined())
			{
				m_items[i]->m_pos.x -= offsets.right.CalcPercent(m_box_right - m_box_left);
			}
			if(!offsets.top.IsPredefined())
			{
				// TODO: m_line_height is not correct here
				m_items[i]->m_pos.y += offsets.top.CalcPercent(m_line_height);
			} else if(!offsets.bottom.IsPredefined())
			{
				// TODO: m_line_height is not correct here
				m_items[i]->m_pos.y -= offsets.bottom.CalcPercent(m_line_height);
			}
		}
	}
	m_height = y2 - y1;
	m_baseline = (baseline - y1) - (m_height - line_height);
}

bool LineBox::CanHold( Element* el, WhiteSpace ws )
{
	if(!el->IsInLineBox()) return false;

	if(el->IsBreak())
	{
		return false;
	}

	if(ws == white_space_nowrap || ws == white_space_pre)
	{
		return true;
	}

	if(m_box_left + m_width + el->Width() + el->GetInlineShiftLeft() + el->GetInlineShiftRight() > m_box_right)
	{
		return false;
	}

	return true;
}

Element* LineBox::GetLastSpace()
{
	Element* ret = 0;
	for(int j = m_items.GetCount() - 1; j >= 0; j--) {
		Element& e = *m_items[j];
		if(e.IsWhiteSpace() || e.IsBreak())
		{
			ret = &e;
		} else
		{
			break;
		}
	}
	return ret;
}

bool LineBox::IsEmpty()
{
	if(m_items.IsEmpty()) return true;
	for(int j = m_items.GetCount() - 1; j >= 0; j--) {
		Element& e = *m_items[j];
		if(!e.m_skip || e.IsBreak())
		{
			return false;
		}
	}
	return true;
}

int LineBox::Baseline()
{
	return m_baseline;
}

void LineBox::GetElements( ElementsVector& els )
{
	for(int i = 0; i < m_items.GetCount(); i++) {
		els.Insert(i, m_items[i]);
	}
}

int LineBox::TopMargin()
{
	return 0;
}

int LineBox::BottomMargin()
{
	return 0;
}

void LineBox::SetYShift( int shift )
{
	m_box_top += shift;
	for (int i = 0; i < m_items.GetCount(); i++) {
		Element& e = *m_items[i];
		e.m_pos.y += shift;
	}
}

bool LineBox::IsBreakOnly()
{
	if(m_items.IsEmpty()) return true;

	if(m_items[0]->IsBreak())
	{
		for (int i = 0; i < m_items.GetCount(); i++) {
			Element& e = *m_items[i];
			if(!e.m_skip)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void LineBox::NewWidth( int left, int right, ElementsVector& els )
{
	int add = left - m_box_left;
	if(add)
	{
		m_box_left	= left;
		m_box_right	= right;
		m_width = 0;
		int remove_begin = m_items.GetCount();
		for (int i = 1; i != m_items.GetCount(); i++) {
			Element* el = m_items[i];

			if(!el->m_skip)
			{
				if(m_box_left + m_width + el->Width() + el->GetInlineShiftRight() + el->GetInlineShiftLeft() > m_box_right)
				{
					remove_begin = i;
					break;
				} else
				{
					el->m_pos.x += add;
					m_width += el->Width() + el->GetInlineShiftRight() + el->GetInlineShiftLeft();
				}
			}
		}
		if(remove_begin != m_items.GetCount())
		{
			for(int i = remove_begin; i < m_items.GetCount(); i++) {
				els.Insert(i, m_items[i]);
			}
			m_items.Remove(remove_begin, m_items.GetCount() - remove_begin);
			for(int i = 0; i < els.GetCount(); i++) {
				els[i]->m_box = 0;
			}
		}
	}
}

END_HTMLCTRL_NAMESPACE
