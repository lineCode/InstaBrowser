#include "Html.h"
#include "ElTd.h"

NAMESPACE_HTMLCTRL

ElTd::ElTd( Document& doc ) : HtmlTag(doc)
{

}

ElTd::~ElTd()
{

}

void ElTd::ParseAttributes()
{
	String str = GetAttr("width");
	if(str.GetCount())
	{
		m_style.AddProperty("width", str, "", false);
	}
	str = GetAttr("background");
	if(str.GetCount())
	
	{
		String url = "url('";
		url += str;
		url += "')";
		m_style.AddProperty("background-image", url, "", false);
	}
	str = GetAttr("align");
	if(str.GetCount())
	{
		m_style.AddProperty("text-align", str, "", false);
	}

	str = GetAttr("bgcolor");
	if (str.GetCount())
	{
		m_style.AddProperty("background-color", str, "", false);
	}

	str = GetAttr("valign");
	if(str.GetCount())
	{
		m_style.AddProperty("vertical-align", str, "", false);
	}
	HtmlTag::ParseAttributes();
}

END_HTMLCTRL_NAMESPACE
