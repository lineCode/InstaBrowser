#include "Html.h"
#include "ElTable.h"
#include "Document.h"
#include "Iterators.h"

#include <Core/Core.h>
using namespace Upp;

NAMESPACE_HTMLCTRL

ElTable::ElTable( Document& doc ) : HtmlTag(doc)
{
	m_border_spacing_x	= 0;
	m_border_spacing_y	= 0;
	m_borderCollapse	= border_collapse_separate;
}


ElTable::~ElTable()
{

}

bool ElTable::AppendChild( Element* el )
{
	if(!el)	return false;
	if(!el->GetTagName().Compare("tbody") || !el->GetTagName().Compare("thead") || !el->GetTagName().Compare("tfoot"))
	{
		return HtmlTag::AppendChild(el);
	}
	return false;
}

void ElTable::ParseStyles(bool is_reparse)
{
	HtmlTag::ParseStyles(is_reparse);

	m_borderCollapse = (BorderCollapse) ValueIndex(GetStyleProperty("border-collapse", true, "separate"), border_collapse_strings, border_collapse_separate);

	if(m_borderCollapse == border_collapse_separate)
	{
		m_css_border_spacing_x.FromString(GetStyleProperty("-litehtml-border-spacing-x", true, "0px"));
		m_css_border_spacing_y.FromString(GetStyleProperty("-litehtml-border-spacing-y", true, "0px"));

		int fntsz = GetFontSize();
		Document* m_doc = GetDocument();
		m_border_spacing_x = m_doc->CvtUnits(m_css_border_spacing_x, fntsz);
		m_border_spacing_y = m_doc->CvtUnits(m_css_border_spacing_y, fntsz);
	} else
	{
		m_border_spacing_x	= 0;
		m_border_spacing_y	= 0;
		m_padding.bottom	= 0;
		m_padding.top		= 0;
		m_padding.left		= 0;
		m_padding.right		= 0;
		m_css_padding.bottom.SetValue(0, css_units_px);
		m_css_padding.top.SetValue(0, css_units_px);
		m_css_padding.left.SetValue(0, css_units_px);
		m_css_padding.right.SetValue(0, css_units_px);
	}
}

void ElTable::ParseAttributes()
{
	String str = GetAttr("width");
	if(str.GetCount())
	{
		m_style.AddProperty("width", str, "", false);
	}

	str = GetAttr("align");
	if(str.GetCount())
	{
		int align = ValueIndex(str, "left;center;right");
		switch(align)
		{
		case 1:
			m_style.AddProperty("margin-left", "auto", "", false);
			m_style.AddProperty("margin-right", "auto", "", false);
			break;
		case 2:
			m_style.AddProperty("margin-left", "auto", "", false);
			m_style.AddProperty("margin-right", "0", "", false);
			break;
		}
	}

	str = GetAttr("cellspacing");
	if(str.GetCount())
	{
		String val = str;
		val += " ";
		val += str;
		m_style.AddProperty("border-spacing", val, "", false);
	}
	
	str = GetAttr("border");
	if(str.GetCount())
	{
		m_style.AddProperty("border-width", str, "", false);
	}

	str = GetAttr("bgcolor");
	if (str.GetCount())
	{
		m_style.AddProperty("background-color", str, "", false);
	}

	HtmlTag::ParseAttributes();
}

END_HTMLCTRL_NAMESPACE
