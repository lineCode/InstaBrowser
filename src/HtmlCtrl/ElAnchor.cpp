#include "Html.h"
#include "ElAnchor.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElAnchor::ElAnchor( Document& doc ) : HtmlTag(doc) {
}

ElAnchor::~ElAnchor() {

}

void ElAnchor::OnClick()
{
	String href = GetAttr("href");
	if (href.GetCount()) {
		GetDocument()->GetHtmlFile()->OnAnchorClick(href, this);
	}
}

void ElAnchor::ApplyStylesheet( const Css& stylesheet )
{
	if( GetAttr("href").GetCount() ) {
		m_pseudo_classes.Add("link");
	}
	HtmlTag::ApplyStylesheet(stylesheet);
}

END_HTMLCTRL_NAMESPACE
