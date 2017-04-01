#include "Html.h"
#include "Iterators.h"
#include "HtmlTag.h"

NAMESPACE_HTMLCTRL

Element* ElementsIterator::Next(bool ret_higher)
{
	NextIdx();

	while(m_idx < (int) m_el->GetChildrenCount())
	{
		Element::ptr el = m_el->GetChild(m_idx);
		if(	el->GetChildrenCount() && m_go_inside && m_go_inside->Select(el) )
		{
			stack_item si;
			si.idx		= m_idx;
			si.el		= m_el;
			m_stack.Add(si);
			m_el		= el;
			m_idx		= -1;
			if(ret_higher)
			{
				return el;
			}
			NextIdx();
		} else
		{
			if( !m_Select || (m_Select && m_Select->Select(m_el->GetChild(m_idx))) )
			{
				return m_el->GetChild(m_idx);
			} else
			{
				NextIdx();
			}
		}
	}

	return 0;
}

void ElementsIterator::NextIdx()
{
	m_idx++;
	while(m_idx >= (int) m_el->GetChildrenCount() && m_stack.GetCount())
	{
		stack_item si = LAST(m_stack);
		REMOVELAST(m_stack);
		m_idx	= si.idx;
		m_el	= si.el;
		m_idx++;
		continue;
	}
}


bool go_inside_inline::Select( Element* el )
{
	if(el->GetDisplay() == display_inline || el->GetDisplay() == display_inline_text)
	{
		return true;
	}
	return false;
}

bool go_inside_table::Select( Element* el )
{
	if(	el->GetDisplay() == display_TableRow_group ||
		el->GetDisplay() == display_table_header_group ||
		el->GetDisplay() == display_table_footer_group)
	{
		return true;
	}
	return false;
}

bool TableRowsSelector::Select( Element* el )
{
	if(	el->GetDisplay() == display_TableRow)
	{
		return true;
	}
	return false;
}

bool TableCellsSelector::Select( Element* el )
{
	if(	el->GetDisplay() == display_TableCell)
	{
		return true;
	}
	return false;
}

END_HTMLCTRL_NAMESPACE
