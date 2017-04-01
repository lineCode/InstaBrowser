#include "Html.h"
#include "ElBase.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElBase::ElBase( Document& doc ) : HtmlTag(doc)
{
	
}

ElBase::~ElBase()
{

}

void ElBase::ParseAttributes()
{
	String href = GetAttr("href");
	if (href.GetCount())
		GetDocument()->GetHtmlFile()->SetBaseUrl(href);
}

END_HTMLCTRL_NAMESPACE
