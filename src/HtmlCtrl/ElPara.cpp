#include "Html.h"
#include "ElPara.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElPara::ElPara( Document& doc ) : HtmlTag(doc)
{
}

ElPara::~ElPara()
{

}

void ElPara::ParseAttributes()
{
	String str = GetAttr("align");
	if(str.GetCount())
	{
		m_style.AddProperty("text-align", str, "", false);
	}

	HtmlTag::ParseAttributes();
}

END_HTMLCTRL_NAMESPACE
