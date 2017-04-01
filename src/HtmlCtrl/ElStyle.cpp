#include "Html.h"
#include "ElStyle.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElStyle::ElStyle( Document& doc ) : Element(doc)
{

}

ElStyle::~ElStyle()
{

}

void ElStyle::ParseAttributes()
{
	String text;

	for(ElementsVector::Iterator iter = m_children.Begin(); iter != m_children.End(); iter++)
	{
		(*iter)->GetText(text);
	}
	
	GetDocument()->AddStylesheet( text, "",  GetAttr("media"));
}

bool ElStyle::AppendChild( Element* el )
{
	m_children.Add(el);
	return true;
}

String ElStyle::GetTagName() const
{
	return "Style";
}

END_HTMLCTRL_NAMESPACE
