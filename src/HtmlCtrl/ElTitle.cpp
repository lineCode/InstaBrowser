#include "Html.h"
#include "ElTitle.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElTitle::ElTitle( Document& doc ) : HtmlTag(doc)
{

}

ElTitle::~ElTitle()
{

}

void ElTitle::ParseAttributes()
{
	String text;
	GetText(text);
	GetDocument()->GetHtmlFile()->SetCaption(text);
}

END_HTMLCTRL_NAMESPACE
