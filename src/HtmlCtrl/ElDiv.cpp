#include "Html.h"
#include "ElDiv.h"

NAMESPACE_HTMLCTRL

ElDiv::ElDiv( Document& doc ) : HtmlTag(doc)
{

}

ElDiv::~ElDiv()
{

}

void ElDiv::ParseAttributes()
{
	String str = GetAttr("align");
	if(str.GetCount()) {
		m_style.AddProperty("text-align", str, "", false);
	}
	HtmlTag::ParseAttributes();
}

END_HTMLCTRL_NAMESPACE
