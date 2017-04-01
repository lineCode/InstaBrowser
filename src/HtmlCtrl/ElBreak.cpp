#include "Html.h"
#include "ElBreak.h"

NAMESPACE_HTMLCTRL

ElBreak::ElBreak( Document& doc ) : HtmlTag(doc)
{

}

ElBreak::~ElBreak()
{

}

bool ElBreak::IsBreak() const
{
	return true;
}

END_HTMLCTRL_NAMESPACE
