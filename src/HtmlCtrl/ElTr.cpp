#include "Html.h"
#include "ElTr.h"

NAMESPACE_HTMLCTRL

ElTr::ElTr( Document& doc ) : HtmlTag(doc)
{

}

ElTr::~ElTr()
{

}

void ElTr::ParseAttributes()
{
	String str = GetAttr("align");
	if(str.GetCount())
	{
		m_style.AddProperty("text-align", str, "", false);
	}
	str = GetAttr("valign");
	if(str.GetCount())
	{
		m_style.AddProperty("vertical-align", str, "", false);
	}
	str = GetAttr("bgcolor");
	if(str.GetCount())
	{
		m_style.AddProperty("background-color", str, "", false);
	}
	HtmlTag::ParseAttributes();
}

void ElTr::GetInLineBoxes( Position::vector& Boxes )
{
	Position pos;
	for(ElementsVector::Iterator iter = m_children.Begin(); iter != m_children.End(); iter++)
	{
		Element* el = (*iter);
		if(el->GetDisplay() == display_TableCell)
		{
			pos.x		= el->Left() + el->MarginLeft();
			pos.y		= el->Top() - m_padding.top - m_borders.top;

			pos.width	= el->Right() - pos.x - el->MarginRight() - el->MarginLeft();
			pos.height	= el->Height() + m_padding.top + m_padding.bottom + m_borders.top + m_borders.bottom;

			Boxes.Add(pos);
		}
	}
}

END_HTMLCTRL_NAMESPACE
