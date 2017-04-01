#include "Html.h"
#include "ElBody.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElBody::ElBody( Document& doc ) : HtmlTag(doc)
{
}

ElBody::~ElBody()
{

}

bool ElBody::IsBody()  const
{
	return true;
}

void ElBody::ParseAttributes()
{
	String str;

	str = GetAttr("bgcolor");
	if (str.GetCount())
	{
		m_style.AddProperty("background-color", str, "", false);
	}

	HtmlTag::ParseAttributes();
}

END_HTMLCTRL_NAMESPACE
