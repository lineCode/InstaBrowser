#include "Html.h"
#include "ElLink.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElLink::ElLink( Document& doc ) : HtmlTag(doc)
{

}

ElLink::~ElLink()
{

}

void ElLink::ParseAttributes()
{
	bool processed = false;

	String rel = GetAttr("rel");
	if(rel.GetCount() && !rel.Compare("stylesheet"))
	{
		String media	= GetAttr("media");
		String href		= GetAttr("href");
		if(href.GetCount() && href[0])
		{
			String css_text;
			String css_baseurl;
			GetDocument()->GetHtmlFile()->ImportCss(css_text, href, css_baseurl);
			if(!css_text.IsEmpty())
			{
				GetDocument()->AddStylesheet(css_text, css_baseurl, media);
				processed = true;
			}
		}
	}

	if(!processed)
	{
		GetDocument()->GetHtmlFile()->Link(GetDocument(), this);
	}
}

END_HTMLCTRL_NAMESPACE
